#include "launcher.h"

#include "injector.h"

#include <algorithm>
#include <filesystem>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <commctrl.h>
#include <commdlg.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <uxtheme.h>

namespace fs = std::filesystem;

namespace thautomation
{
	namespace
	{
		constexpr wchar_t window_class_name[] = L"THAutomationLauncherWindow";
		constexpr wchar_t registry_path[] = L"Software\\THAutomation";
		constexpr UINT message_process_exit = WM_APP + 1;

		constexpr int id_game = 1001;
		constexpr int id_executable = 1002;
		constexpr int id_browse = 1003;
		constexpr int id_launch = 1004;
		constexpr int id_status = 1005;
		constexpr int id_log = 1006;
		constexpr int id_minimize = 1007;
		constexpr int id_legacy = 1008;
		constexpr int id_open_folder = 1009;
		constexpr int id_detect = 1010;
		constexpr int id_runtime = 1011;
		constexpr int id_arguments = 1012;

		constexpr COLORREF color_background = RGB(10, 15, 28);
		constexpr COLORREF color_sidebar = RGB(16, 24, 45);
		constexpr COLORREF color_surface = RGB(20, 29, 48);
		constexpr COLORREF color_surface_hover = RGB(30, 42, 66);
		constexpr COLORREF color_field = RGB(12, 20, 35);
		constexpr COLORREF color_border = RGB(45, 60, 88);
		constexpr COLORREF color_text = RGB(235, 241, 252);
		constexpr COLORREF color_muted = RGB(145, 159, 184);
		constexpr COLORREF color_accent = RGB(91, 132, 255);
		constexpr COLORREF color_accent_hot = RGB(113, 150, 255);
		constexpr COLORREF color_success = RGB(72, 211, 147);
		constexpr COLORREF color_warning = RGB(250, 190, 74);
		constexpr COLORREF color_error = RGB(255, 101, 119);

		struct app_state
		{
			HINSTANCE instance = nullptr;
			HWND window = nullptr;
			HWND game = nullptr;
			HWND executable = nullptr;
			HWND browse = nullptr;
			HWND launch = nullptr;
			HWND status = nullptr;
			HWND log = nullptr;
			HWND minimize = nullptr;
			HWND legacy = nullptr;
			HWND open_folder = nullptr;
			HWND detect = nullptr;
			HWND runtime = nullptr;
			HWND arguments = nullptr;

			HFONT font_normal = nullptr;
			HFONT font_small = nullptr;
			HFONT font_medium = nullptr;
			HFONT font_title = nullptr;
			HFONT font_logo = nullptr;
			HBRUSH background_brush = nullptr;
			HBRUSH sidebar_brush = nullptr;
			HBRUSH field_brush = nullptr;

			bool ready = false;
			bool warning = false;
			bool running = false;
			validation_report validation;
			fs::path module_directory;
		};

		int scale(const app_state& state, const int value)
		{
			const UINT dpi = GetDpiForWindow(state.window);
			return MulDiv(value, static_cast<int>(dpi ? dpi : 96), 96);
		}

		std::wstring window_text(const HWND window)
		{
			const int length = GetWindowTextLengthW(window);
			std::wstring value(static_cast<size_t>(length + 1), L'\0');
			GetWindowTextW(window, value.data(), length + 1);
			value.resize(static_cast<size_t>(length));
			return value;
		}

		fs::path executable_directory()
		{
			std::vector<wchar_t> buffer(32768);
			const DWORD length = GetModuleFileNameW(nullptr, buffer.data(),
				static_cast<DWORD>(buffer.size()));
			if (length == 0 || length >= buffer.size())
				return fs::current_path();
			return fs::path(std::wstring(buffer.data(), length)).parent_path();
		}

		std::wstring read_string_setting(const std::wstring& name,
			const std::wstring& fallback = L"")
		{
			HKEY key = nullptr;
			if (RegOpenKeyExW(HKEY_CURRENT_USER, registry_path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
				return fallback;

			DWORD type = 0;
			DWORD bytes = 0;
			if (RegQueryValueExW(key, name.c_str(), nullptr, &type, nullptr, &bytes) != ERROR_SUCCESS ||
				(type != REG_SZ && type != REG_EXPAND_SZ) || bytes < sizeof(wchar_t))
			{
				RegCloseKey(key);
				return fallback;
			}

			std::wstring value(bytes / sizeof(wchar_t), L'\0');
			if (RegQueryValueExW(key, name.c_str(), nullptr, nullptr,
				reinterpret_cast<BYTE*>(value.data()), &bytes) != ERROR_SUCCESS)
			{
				RegCloseKey(key);
				return fallback;
			}
			RegCloseKey(key);
			while (!value.empty() && value.back() == L'\0')
				value.pop_back();
			return value;
		}

		void write_string_setting(const std::wstring& name, const std::wstring& value)
		{
			HKEY key = nullptr;
			if (RegCreateKeyExW(HKEY_CURRENT_USER, registry_path, 0, nullptr, 0,
				KEY_SET_VALUE, nullptr, &key, nullptr) != ERROR_SUCCESS)
				return;
			RegSetValueExW(key, name.c_str(), 0, REG_SZ,
				reinterpret_cast<const BYTE*>(value.c_str()),
				static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t)));
			RegCloseKey(key);
		}

		DWORD read_dword_setting(const std::wstring& name, const DWORD fallback)
		{
			HKEY key = nullptr;
			if (RegOpenKeyExW(HKEY_CURRENT_USER, registry_path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
				return fallback;

			DWORD value = fallback;
			DWORD type = REG_DWORD;
			DWORD bytes = sizeof(value);
			if (RegQueryValueExW(key, name.c_str(), nullptr, &type,
				reinterpret_cast<BYTE*>(&value), &bytes) != ERROR_SUCCESS || type != REG_DWORD)
				value = fallback;
			RegCloseKey(key);
			return value;
		}

		void write_dword_setting(const std::wstring& name, const DWORD value)
		{
			HKEY key = nullptr;
			if (RegCreateKeyExW(HKEY_CURRENT_USER, registry_path, 0, nullptr, 0,
				KEY_SET_VALUE, nullptr, &key, nullptr) != ERROR_SUCCESS)
				return;
			RegSetValueExW(key, name.c_str(), 0, REG_DWORD,
				reinterpret_cast<const BYTE*>(&value), sizeof(value));
			RegCloseKey(key);
		}

		const game_definition* selected_game(const app_state& state)
		{
			const LRESULT selected = SendMessageW(state.game, CB_GETCURSEL, 0, 0);
			if (selected == CB_ERR)
				return nullptr;
			const LRESULT game_index = SendMessageW(state.game, CB_GETITEMDATA,
				static_cast<WPARAM>(selected), 0);
			const auto& games = supported_games();
			if (game_index < 0 || static_cast<size_t>(game_index) >= games.size())
				return nullptr;
			return &games[static_cast<size_t>(game_index)];
		}

		std::optional<std::pair<size_t, fs::path>> detect_game(
			const app_state& state, std::optional<size_t> only_index);

		std::wstring game_path_setting(const game_definition& game)
		{
			return L"path_" + game.id;
		}

		launch_request current_request(const app_state& state)
		{
			launch_request request{};
			request.game = selected_game(state);
			request.executable_path = window_text(state.executable);
			request.hook_dll_path = state.module_directory / L"twinhook.dll";
			request.legacy_renderer_directory = state.module_directory / L"legacy";
			request.arguments = window_text(state.arguments);
			request.install_legacy_renderer =
				SendMessageW(state.legacy, BM_GETCHECK, 0, 0) == BST_CHECKED;
			return request;
		}

		void append_log(app_state& state, const std::wstring& message)
		{
			SYSTEMTIME time{};
			GetLocalTime(&time);
			wchar_t prefix[32]{};
			swprintf_s(prefix, L"[%02u:%02u:%02u] ", time.wHour, time.wMinute, time.wSecond);
			std::wstring line = prefix + message + L"\r\n";

			const int length = GetWindowTextLengthW(state.log);
			SendMessageW(state.log, EM_SETSEL, length, length);
			SendMessageW(state.log, EM_REPLACESEL, FALSE,
				reinterpret_cast<LPARAM>(line.c_str()));
			SendMessageW(state.log, EM_SCROLLCARET, 0, 0);
		}

		void update_validation(app_state& state)
		{
			state.validation = validate_launch_request(current_request(state));
			state.ready = state.validation.ready;
			state.warning = !state.validation.warnings.empty();
			SetWindowTextW(state.status, state.validation.summary.c_str());
			EnableWindow(state.launch, state.ready && !state.running);
			InvalidateRect(state.status, nullptr, TRUE);
			InvalidateRect(state.launch, nullptr, TRUE);
		}

		void select_game_index(app_state& state, const size_t index, const bool load_saved_path)
		{
			if (index >= supported_games().size())
				return;
			SendMessageW(state.game, CB_SETCURSEL, index, 0);
			const auto& game = supported_games()[index];
			if (load_saved_path)
			{
				std::wstring path = read_string_setting(game_path_setting(game));
				if (path.empty())
				{
					if (auto detection = detect_game(state, index))
					{
						path = detection->second.wstring();
						write_string_setting(game_path_setting(game), path);
					}
				}
				SetWindowTextW(state.executable, path.c_str());
			}
			EnableWindow(state.legacy, game.requires_legacy_renderer && !state.running);
			ShowWindow(state.legacy, game.requires_legacy_renderer ? SW_SHOW : SW_HIDE);
			write_string_setting(L"last_game", game.id);
			update_validation(state);
		}

		std::vector<fs::path> search_roots(const app_state& state)
		{
			std::vector<fs::path> roots;
			auto add = [&roots](const fs::path& path) {
				if (path.empty())
					return;
				for (const auto& existing : roots)
					if (_wcsicmp(existing.c_str(), path.c_str()) == 0)
						return;
				roots.push_back(path);
			};
			add(state.module_directory);
			std::error_code error;
			add(fs::current_path(error));
			add(state.module_directory.parent_path());
			return roots;
		}

		std::optional<std::pair<size_t, fs::path>> detect_game(
			const app_state& state, const std::optional<size_t> only_index)
		{
			const auto& games = supported_games();
			const auto roots = search_roots(state);
			for (size_t game_index = 0; game_index < games.size(); ++game_index)
			{
				if (only_index && *only_index != game_index)
					continue;
				const auto& game = games[game_index];

				const std::wstring saved = read_string_setting(game_path_setting(game));
				std::error_code error;
				if (!saved.empty() && fs::is_regular_file(saved, error))
					return std::make_pair(game_index, fs::path(saved));

				for (const auto& root : roots)
				{
					for (const auto& name : game.executable_names)
					{
						const fs::path direct = root / name;
						error.clear();
						if (fs::is_regular_file(direct, error))
							return std::make_pair(game_index, direct);
					}

					error.clear();
					for (fs::directory_iterator iterator(root,
						fs::directory_options::skip_permission_denied, error), end;
						iterator != end && !error; iterator.increment(error))
					{
						if (!iterator->is_directory(error))
							continue;
						for (const auto& name : game.executable_names)
						{
							const fs::path nested = iterator->path() / name;
							error.clear();
							if (fs::is_regular_file(nested, error))
								return std::make_pair(game_index, nested);
						}
					}
				}
			}
			return std::nullopt;
		}

		void apply_detected_game(app_state& state,
			const std::pair<size_t, fs::path>& detection, const bool announce)
		{
			select_game_index(state, detection.first, false);
			SetWindowTextW(state.executable, detection.second.c_str());
			const auto& game = supported_games()[detection.first];
			write_string_setting(game_path_setting(game), detection.second.wstring());
			if (announce)
				append_log(state, L"已识别 " + game.display_name + L"：" + detection.second.wstring());
			update_validation(state);
		}

		void browse_for_executable(app_state& state)
		{
			std::vector<wchar_t> filename(32768);
			const std::wstring current = window_text(state.executable);
			if (!current.empty())
				wcsncpy_s(filename.data(), filename.size(), current.c_str(), _TRUNCATE);

			std::wstring initial_directory;
			if (!current.empty())
				initial_directory = fs::path(current).parent_path().wstring();

			OPENFILENAMEW dialog{};
			dialog.lStructSize = sizeof(dialog);
			dialog.hwndOwner = state.window;
			dialog.lpstrFilter =
				L"Touhou 游戏程序 (*.exe)\0*.exe\0所有文件 (*.*)\0*.*\0\0";
			dialog.lpstrFile = filename.data();
			dialog.nMaxFile = static_cast<DWORD>(filename.size());
			dialog.lpstrTitle = L"选择要注入并启动的游戏";
			dialog.lpstrInitialDir = initial_directory.empty() ? nullptr : initial_directory.c_str();
			dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST |
				OFN_HIDEREADONLY | OFN_DONTADDTORECENT;

			if (!GetOpenFileNameW(&dialog))
				return;

			const fs::path chosen(filename.data());
			if (const auto* detected = find_game_for_executable(chosen))
			{
				const auto& games = supported_games();
				const size_t index = static_cast<size_t>(detected - games.data());
				select_game_index(state, index, false);
			}
			SetWindowTextW(state.executable, chosen.c_str());
			if (const auto* game = selected_game(state))
				write_string_setting(game_path_setting(*game), chosen.wstring());
			append_log(state, L"已选择游戏：" + chosen.wstring());
			update_validation(state);
		}

		void open_game_folder(app_state& state)
		{
			const fs::path executable = window_text(state.executable);
			std::error_code error;
			if (!fs::is_regular_file(executable, error))
			{
				MessageBoxW(state.window, L"请先选择有效的游戏 EXE。", L"THAutomation",
					MB_OK | MB_ICONINFORMATION);
				return;
			}
			ShellExecuteW(state.window, L"open", executable.parent_path().c_str(),
				nullptr, nullptr, SW_SHOWNORMAL);
		}

		void set_running_state(app_state& state, const bool running)
		{
			state.running = running;
			EnableWindow(state.game, !running);
			EnableWindow(state.executable, !running);
			EnableWindow(state.browse, !running);
			EnableWindow(state.detect, !running);
			EnableWindow(state.arguments, !running);
			EnableWindow(state.legacy,
				!running && selected_game(state) && selected_game(state)->requires_legacy_renderer);
			SetWindowTextW(state.launch, running ? L"游戏运行中" : L"一键注入并启动");
			if (!running)
				update_validation(state);
			else
			{
				EnableWindow(state.launch, FALSE);
				InvalidateRect(state.launch, nullptr, TRUE);
			}
		}

		void launch_game(app_state& state)
		{
			update_validation(state);
			if (!state.validation.ready)
			{
				MessageBoxW(state.window, state.validation.summary.c_str(),
					L"暂时无法启动", MB_OK | MB_ICONWARNING);
				return;
			}

			for (const auto& warning : state.validation.warnings)
				append_log(state, L"兼容性提示：" + warning);

			const launch_request request = current_request(state);
			if (request.game)
			{
				write_string_setting(game_path_setting(*request.game),
					request.executable_path.wstring());
				write_string_setting(L"last_game", request.game->id);
			}
			write_string_setting(L"arguments", request.arguments);
			write_dword_setting(L"minimize_after_launch",
				SendMessageW(state.minimize, BM_GETCHECK, 0, 0) == BST_CHECKED);
			write_dword_setting(L"install_legacy_renderer",
				SendMessageW(state.legacy, BM_GETCHECK, 0, 0) == BST_CHECKED);

			append_log(state, L"正在启动 " + request.game->display_name + L"…");
			set_running_state(state, true);
			const launch_result result = launch_injected(request);
			if (!result.success)
			{
				set_running_state(state, false);
				append_log(state, L"失败：" + result.message);
				MessageBoxW(state.window, result.message.c_str(),
					L"注入失败", MB_OK | MB_ICONERROR);
				return;
			}

			state.ready = true;
			state.warning = false;
			SetWindowTextW(state.status, L"注入成功，游戏正在运行。");
			InvalidateRect(state.status, nullptr, TRUE);
			append_log(state, L"注入成功，进程 PID " + std::to_wstring(result.process_id) + L"。");

			const HWND window = state.window;
			const HANDLE process = result.process_handle;
			const DWORD process_id = result.process_id;
			std::thread([window, process, process_id]() {
				WaitForSingleObject(process, INFINITE);
				DWORD exit_code = 0;
				GetExitCodeProcess(process, &exit_code);
				CloseHandle(process);
				PostMessageW(window, message_process_exit,
					static_cast<WPARAM>(process_id), static_cast<LPARAM>(exit_code));
			}).detach();

			if (SendMessageW(state.minimize, BM_GETCHECK, 0, 0) == BST_CHECKED)
				ShowWindow(state.window, SW_MINIMIZE);
		}

		HFONT create_font(const int size, const int weight)
		{
			return CreateFontW(-size, 0, 0, 0, weight, FALSE, FALSE, FALSE,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
		}

		void set_font(const HWND window, const HFONT font)
		{
			SendMessageW(window, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
		}

		HWND create_label(app_state& state, const wchar_t* text,
			const int x, const int y, const int width, const int height)
		{
			HWND label = CreateWindowExW(0, L"STATIC", text,
				WS_CHILD | WS_VISIBLE | SS_LEFT,
				scale(state, x), scale(state, y), scale(state, width), scale(state, height),
				state.window, nullptr, state.instance, nullptr);
			set_font(label, state.font_small);
			return label;
		}

		HWND create_button(app_state& state, const int id, const wchar_t* text,
			const int x, const int y, const int width, const int height)
		{
			HWND button = CreateWindowExW(0, L"BUTTON", text,
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
				scale(state, x), scale(state, y), scale(state, width), scale(state, height),
				state.window, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
				state.instance, nullptr);
			set_font(button, state.font_medium);
			return button;
		}

		void create_controls(app_state& state)
		{
			const UINT dpi = GetDpiForWindow(state.window);
			state.font_normal = create_font(MulDiv(16, dpi, 96), FW_NORMAL);
			state.font_small = create_font(MulDiv(14, dpi, 96), FW_NORMAL);
			state.font_medium = create_font(MulDiv(16, dpi, 96), FW_SEMIBOLD);
			state.font_title = create_font(MulDiv(26, dpi, 96), FW_BOLD);
			state.font_logo = create_font(MulDiv(38, dpi, 96), FW_BOLD);
			state.background_brush = CreateSolidBrush(color_background);
			state.sidebar_brush = CreateSolidBrush(color_sidebar);
			state.field_brush = CreateSolidBrush(color_field);

			state.status = CreateWindowExW(0, L"STATIC", L"正在检查…",
				WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
				scale(state, 282), scale(state, 34), scale(state, 578), scale(state, 58),
				state.window, reinterpret_cast<HMENU>(id_status), state.instance, nullptr);

			create_label(state, L"游戏版本", 282, 112, 180, 22);
			state.game = CreateWindowExW(0, WC_COMBOBOXW, nullptr,
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST |
				CBS_OWNERDRAWFIXED | CBS_HASSTRINGS,
				scale(state, 282), scale(state, 137), scale(state, 578), scale(state, 260),
				state.window, reinterpret_cast<HMENU>(id_game), state.instance, nullptr);
			set_font(state.game, state.font_normal);
			SendMessageW(state.game, CB_SETITEMHEIGHT, static_cast<WPARAM>(-1), scale(state, 34));
			SendMessageW(state.game, CB_SETITEMHEIGHT, 0, scale(state, 34));
			SetWindowTheme(state.game, L"DarkMode_CFD", nullptr);

			const auto& games = supported_games();
			for (size_t index = 0; index < games.size(); ++index)
			{
				const LRESULT item = SendMessageW(state.game, CB_ADDSTRING, 0,
					reinterpret_cast<LPARAM>(games[index].display_name.c_str()));
				SendMessageW(state.game, CB_SETITEMDATA, static_cast<WPARAM>(item),
					static_cast<LPARAM>(index));
			}

			create_label(state, L"游戏程序", 282, 194, 180, 22);
			state.executable = CreateWindowExW(0, L"EDIT", nullptr,
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL,
				scale(state, 282), scale(state, 219), scale(state, 454), scale(state, 39),
				state.window, reinterpret_cast<HMENU>(id_executable), state.instance, nullptr);
			set_font(state.executable, state.font_normal);
			SendMessageW(state.executable, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN,
				MAKELPARAM(scale(state, 10), scale(state, 10)));

			state.browse = create_button(state, id_browse, L"浏览…", 748, 219, 112, 39);

			state.minimize = CreateWindowExW(0, L"BUTTON", L"启动后最小化",
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
				scale(state, 282), scale(state, 276), scale(state, 185), scale(state, 26),
				state.window, reinterpret_cast<HMENU>(id_minimize), state.instance, nullptr);
			set_font(state.minimize, state.font_small);
			SendMessageW(state.minimize, BM_SETCHECK,
				read_dword_setting(L"minimize_after_launch", 1) ? BST_CHECKED : BST_UNCHECKED, 0);

			state.legacy = CreateWindowExW(0, L"BUTTON", L"自动安装旧版 DX 兼容层",
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
				scale(state, 485), scale(state, 276), scale(state, 270), scale(state, 26),
				state.window, reinterpret_cast<HMENU>(id_legacy), state.instance, nullptr);
			set_font(state.legacy, state.font_small);
			SendMessageW(state.legacy, BM_SETCHECK,
				read_dword_setting(L"install_legacy_renderer", 1) ? BST_CHECKED : BST_UNCHECKED, 0);

			state.launch = create_button(state, id_launch, L"一键注入并启动",
				282, 320, 578, 60);
			state.open_folder = create_button(state, id_open_folder, L"打开游戏目录",
				282, 397, 176, 40);
			state.detect = create_button(state, id_detect, L"自动识别游戏",
				473, 397, 176, 40);
			state.runtime = create_button(state, id_runtime, L"安装运行库",
				664, 397, 196, 40);

			create_label(state, L"高级：启动参数（可留空）", 282, 455, 260, 22);
			state.arguments = CreateWindowExW(0, L"EDIT",
				read_string_setting(L"arguments").c_str(),
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL,
				scale(state, 282), scale(state, 480), scale(state, 578), scale(state, 37),
				state.window, reinterpret_cast<HMENU>(id_arguments), state.instance, nullptr);
			set_font(state.arguments, state.font_normal);
			SendMessageW(state.arguments, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN,
				MAKELPARAM(scale(state, 10), scale(state, 10)));

			create_label(state, L"运行日志", 282, 535, 180, 22);
			state.log = CreateWindowExW(0, L"EDIT", nullptr,
				WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY |
				ES_AUTOVSCROLL | WS_VSCROLL,
				scale(state, 282), scale(state, 560), scale(state, 578), scale(state, 72),
				state.window, reinterpret_cast<HMENU>(id_log), state.instance, nullptr);
			set_font(state.log, state.font_small);
			SendMessageW(state.log, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN,
				MAKELPARAM(scale(state, 8), scale(state, 8)));

			const std::wstring last_game = read_string_setting(L"last_game", L"th10");
			size_t selected_index = 0;
			for (size_t index = 0; index < games.size(); ++index)
				if (games[index].id == last_game)
					selected_index = index;
			select_game_index(state, selected_index, true);

			if (window_text(state.executable).empty())
			{
				if (auto detected = detect_game(state, selected_index))
					apply_detected_game(state, *detected, false);
			}

			append_log(state, L"启动器已就绪。选择游戏，或把游戏 EXE 拖到窗口中。");
			DragAcceptFiles(state.window, TRUE);
			update_validation(state);
		}

		void draw_button(app_state& state, const DRAWITEMSTRUCT& item)
		{
			RECT rect = item.rcItem;
			const bool disabled = (item.itemState & ODS_DISABLED) != 0;
			const bool pressed = (item.itemState & ODS_SELECTED) != 0;
			const bool primary = item.CtlID == id_launch;
			const COLORREF fill = disabled ? RGB(48, 58, 76)
				: primary ? (pressed ? color_accent_hot : color_accent)
				: (pressed ? color_surface_hover : color_surface);

			HBRUSH brush = CreateSolidBrush(fill);
			HPEN pen = CreatePen(PS_SOLID, 1, primary ? fill : color_border);
			const HGDIOBJ old_brush = SelectObject(item.hDC, brush);
			const HGDIOBJ old_pen = SelectObject(item.hDC, pen);
			RoundRect(item.hDC, rect.left, rect.top, rect.right, rect.bottom,
				scale(state, 10), scale(state, 10));
			SelectObject(item.hDC, old_brush);
			SelectObject(item.hDC, old_pen);
			DeleteObject(brush);
			DeleteObject(pen);

			wchar_t text[128]{};
			GetWindowTextW(item.hwndItem, text, static_cast<int>(std::size(text)));
			SetBkMode(item.hDC, TRANSPARENT);
			SetTextColor(item.hDC, disabled ? color_muted : color_text);
			SelectObject(item.hDC, state.font_medium);
			DrawTextW(item.hDC, text, -1, &rect,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
		}

		void draw_status(app_state& state, const DRAWITEMSTRUCT& item)
		{
			RECT rect = item.rcItem;
			HBRUSH brush = CreateSolidBrush(color_surface);
			HPEN pen = CreatePen(PS_SOLID, 1, color_border);
			const HGDIOBJ old_brush = SelectObject(item.hDC, brush);
			const HGDIOBJ old_pen = SelectObject(item.hDC, pen);
			RoundRect(item.hDC, rect.left, rect.top, rect.right, rect.bottom,
				scale(state, 12), scale(state, 12));
			SelectObject(item.hDC, old_brush);
			SelectObject(item.hDC, old_pen);
			DeleteObject(brush);
			DeleteObject(pen);

			const COLORREF indicator = state.running || (state.ready && !state.warning)
				? color_success : state.ready ? color_warning : color_error;
			HBRUSH indicator_brush = CreateSolidBrush(indicator);
			SelectObject(item.hDC, indicator_brush);
			SelectObject(item.hDC, GetStockObject(NULL_PEN));
			const int radius = scale(state, 5);
			const int center_x = rect.left + scale(state, 22);
			const int center_y = (rect.top + rect.bottom) / 2;
			Ellipse(item.hDC, center_x - radius, center_y - radius,
				center_x + radius, center_y + radius);
			DeleteObject(indicator_brush);

			wchar_t text[512]{};
			GetWindowTextW(item.hwndItem, text, static_cast<int>(std::size(text)));
			rect.left += scale(state, 40);
			rect.right -= scale(state, 16);
			SetBkMode(item.hDC, TRANSPARENT);
			SetTextColor(item.hDC, color_text);
			SelectObject(item.hDC, state.font_normal);
			DrawTextW(item.hDC, text, -1, &rect,
				DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
		}

		void draw_combo_item(app_state& state, const DRAWITEMSTRUCT& item)
		{
			if (item.itemID == static_cast<UINT>(-1))
				return;
			const bool selected = (item.itemState & ODS_SELECTED) != 0;
			HBRUSH brush = CreateSolidBrush(selected ? color_surface_hover : color_field);
			FillRect(item.hDC, &item.rcItem, brush);
			DeleteObject(brush);

			const LRESULT game_index = SendMessageW(item.hwndItem, CB_GETITEMDATA,
				item.itemID, 0);
			const auto& games = supported_games();
			if (game_index < 0 || static_cast<size_t>(game_index) >= games.size())
				return;

			RECT text_rect = item.rcItem;
			text_rect.left += scale(state, 12);
			text_rect.right -= scale(state, 8);
			SetBkMode(item.hDC, TRANSPARENT);
			SetTextColor(item.hDC, color_text);
			SelectObject(item.hDC, state.font_normal);
			DrawTextW(item.hDC, games[static_cast<size_t>(game_index)].display_name.c_str(),
				-1, &text_rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
		}

		void paint_window(app_state& state)
		{
			PAINTSTRUCT paint{};
			HDC dc = BeginPaint(state.window, &paint);
			RECT client{};
			GetClientRect(state.window, &client);
			FillRect(dc, &client, state.background_brush);

			RECT sidebar{0, 0, scale(state, 250), client.bottom};
			FillRect(dc, &sidebar, state.sidebar_brush);
			SetBkMode(dc, TRANSPARENT);

			HBRUSH logo_brush = CreateSolidBrush(color_accent);
			HPEN no_pen = static_cast<HPEN>(GetStockObject(NULL_PEN));
			const HGDIOBJ old_brush = SelectObject(dc, logo_brush);
			const HGDIOBJ old_pen = SelectObject(dc, no_pen);
			Ellipse(dc, scale(state, 34), scale(state, 38),
				scale(state, 106), scale(state, 110));
			SelectObject(dc, old_brush);
			SelectObject(dc, old_pen);
			DeleteObject(logo_brush);

			RECT logo_text{scale(state, 34), scale(state, 38),
				scale(state, 106), scale(state, 110)};
			SelectObject(dc, state.font_logo);
			SetTextColor(dc, RGB(255, 255, 255));
			DrawTextW(dc, L"TH", -1, &logo_text,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			RECT brand{scale(state, 34), scale(state, 128),
				scale(state, 222), scale(state, 178)};
			SelectObject(dc, state.font_title);
			SetTextColor(dc, color_text);
			DrawTextW(dc, L"THAutomation", -1, &brand,
				DT_LEFT | DT_TOP | DT_SINGLELINE);

			RECT subtitle{scale(state, 34), scale(state, 174),
				scale(state, 218), scale(state, 218)};
			SelectObject(dc, state.font_small);
			SetTextColor(dc, color_muted);
			DrawTextW(dc, L"Touhou 自动化启动器", -1, &subtitle,
				DT_LEFT | DT_TOP | DT_SINGLELINE);

			RECT steps{scale(state, 34), scale(state, 252),
				scale(state, 218), scale(state, 442)};
			SetTextColor(dc, color_text);
			DrawTextW(dc,
				L"1  选择游戏版本\r\n\r\n"
				L"2  选择或拖入游戏 EXE\r\n\r\n"
				L"3  点击一键注入并启动",
				-1, &steps, DT_LEFT | DT_TOP | DT_WORDBREAK);

			RECT note{scale(state, 34), scale(state, 520),
				scale(state, 218), scale(state, 620)};
			SetTextColor(dc, color_muted);
			DrawTextW(dc,
				L"路径会自动记忆。\r\n下次打开只需点击一次。",
				-1, &note, DT_LEFT | DT_TOP | DT_WORDBREAK);

			EndPaint(state.window, &paint);
		}

		LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
		{
			auto* state = reinterpret_cast<app_state*>(GetWindowLongPtrW(window, GWLP_USERDATA));
			if (message == WM_NCCREATE)
			{
				auto* create = reinterpret_cast<CREATESTRUCTW*>(l_param);
				state = static_cast<app_state*>(create->lpCreateParams);
				state->window = window;
				SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
			}

			if (!state)
				return DefWindowProcW(window, message, w_param, l_param);

			switch (message)
			{
			case WM_CREATE:
			{
				BOOL dark = TRUE;
				DwmSetWindowAttribute(window, 20, &dark, sizeof(dark));
				create_controls(*state);
				return 0;
			}
			case WM_PAINT:
				paint_window(*state);
				return 0;
			case WM_ERASEBKGND:
				return 1;
			case WM_CTLCOLORSTATIC:
			case WM_CTLCOLORBTN:
			{
				HDC dc = reinterpret_cast<HDC>(w_param);
				SetBkMode(dc, TRANSPARENT);
				SetTextColor(dc, IsWindowEnabled(reinterpret_cast<HWND>(l_param))
					? color_text : color_muted);
				return reinterpret_cast<LRESULT>(state->background_brush);
			}
			case WM_CTLCOLOREDIT:
			case WM_CTLCOLORLISTBOX:
			{
				HDC dc = reinterpret_cast<HDC>(w_param);
				SetBkColor(dc, color_field);
				SetTextColor(dc, color_text);
				return reinterpret_cast<LRESULT>(state->field_brush);
			}
			case WM_MEASUREITEM:
			{
				auto* item = reinterpret_cast<MEASUREITEMSTRUCT*>(l_param);
				if (item->CtlID == id_game)
				{
					item->itemHeight = scale(*state, 34);
					return TRUE;
				}
				break;
			}
			case WM_DRAWITEM:
			{
				const auto* item = reinterpret_cast<DRAWITEMSTRUCT*>(l_param);
				if (item->CtlID == id_status)
					draw_status(*state, *item);
				else if (item->CtlID == id_game)
					draw_combo_item(*state, *item);
				else if (item->CtlType == ODT_BUTTON)
					draw_button(*state, *item);
				else
					break;
				return TRUE;
			}
			case WM_COMMAND:
			{
				const int id = LOWORD(w_param);
				const int notification = HIWORD(w_param);
				if (id == id_game && notification == CBN_SELCHANGE)
				{
					const LRESULT index = SendMessageW(state->game, CB_GETCURSEL, 0, 0);
					if (index != CB_ERR)
						select_game_index(*state, static_cast<size_t>(index), true);
				}
				else if (id == id_executable && notification == EN_CHANGE)
					update_validation(*state);
				else if (id == id_browse && notification == BN_CLICKED)
					browse_for_executable(*state);
				else if (id == id_launch && notification == BN_CLICKED)
					launch_game(*state);
				else if (id == id_open_folder && notification == BN_CLICKED)
					open_game_folder(*state);
				else if (id == id_detect && notification == BN_CLICKED)
				{
					const auto* game = selected_game(*state);
					const auto& games = supported_games();
					const size_t game_index = game
						? static_cast<size_t>(game - games.data()) : 0;
					if (auto detection = detect_game(*state, game_index))
						apply_detected_game(*state, *detection, true);
					else
						MessageBoxW(window,
							L"没有在启动器附近发现所选版本。\n请点击“浏览…”或把游戏 EXE 拖到窗口中。",
							L"未发现游戏", MB_OK | MB_ICONINFORMATION);
				}
				else if (id == id_runtime && notification == BN_CLICKED)
				{
					ShellExecuteW(window, L"open",
						L"https://www.microsoft.com/en-us/download/details.aspx?id=8109",
						nullptr, nullptr, SW_SHOWNORMAL);
				}
				else if ((id == id_minimize || id == id_legacy) && notification == BN_CLICKED)
				{
					write_dword_setting(id == id_minimize
						? L"minimize_after_launch" : L"install_legacy_renderer",
						SendMessageW(reinterpret_cast<HWND>(l_param), BM_GETCHECK, 0, 0) == BST_CHECKED);
					update_validation(*state);
				}
				return 0;
			}
			case WM_DROPFILES:
			{
				HDROP drop = reinterpret_cast<HDROP>(w_param);
				wchar_t path[32768]{};
				if (DragQueryFileW(drop, 0, path, static_cast<UINT>(std::size(path))) > 0)
				{
					const fs::path dropped(path);
					if (_wcsicmp(dropped.extension().c_str(), L".exe") == 0)
					{
						if (const auto* detected = find_game_for_executable(dropped))
						{
							const auto& games = supported_games();
							select_game_index(*state,
								static_cast<size_t>(detected - games.data()), false);
						}
						SetWindowTextW(state->executable, dropped.c_str());
						if (const auto* game = selected_game(*state))
							write_string_setting(game_path_setting(*game), dropped.wstring());
						append_log(*state, L"已拖入游戏：" + dropped.wstring());
						update_validation(*state);
					}
				}
				DragFinish(drop);
				return 0;
			}
			case message_process_exit:
			{
				const DWORD process_id = static_cast<DWORD>(w_param);
				const DWORD exit_code = static_cast<DWORD>(l_param);
				set_running_state(*state, false);
				SetWindowTextW(state->status, L"游戏已退出，可以再次启动。");
				state->ready = true;
				state->warning = false;
				InvalidateRect(state->status, nullptr, TRUE);
				append_log(*state, L"游戏进程 " + std::to_wstring(process_id) +
					L" 已退出，代码 " + std::to_wstring(exit_code) + L"。");
				if (IsIconic(window))
				{
					FLASHWINFO flash{sizeof(flash), window, FLASHW_TRAY, 3, 0};
					FlashWindowEx(&flash);
				}
				return 0;
			}
			case WM_DESTROY:
				DeleteObject(state->font_normal);
				DeleteObject(state->font_small);
				DeleteObject(state->font_medium);
				DeleteObject(state->font_title);
				DeleteObject(state->font_logo);
				DeleteObject(state->background_brush);
				DeleteObject(state->sidebar_brush);
				DeleteObject(state->field_brush);
				PostQuitMessage(0);
				return 0;
			default:
				break;
			}
			return DefWindowProcW(window, message, w_param, l_param);
		}
	}

	int run_launcher(HINSTANCE instance, const int show_command)
	{
		const HANDLE instance_mutex = CreateMutexW(nullptr, FALSE,
			L"Local\\THAutomationLauncherSingleton");
		if (instance_mutex && GetLastError() == ERROR_ALREADY_EXISTS)
		{
			if (HWND existing = FindWindowW(window_class_name, nullptr))
			{
				ShowWindow(existing, SW_RESTORE);
				SetForegroundWindow(existing);
			}
			CloseHandle(instance_mutex);
			return 0;
		}

		INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_STANDARD_CLASSES};
		InitCommonControlsEx(&controls);

		WNDCLASSEXW window_class{};
		window_class.cbSize = sizeof(window_class);
		window_class.style = CS_HREDRAW | CS_VREDRAW;
		window_class.lpfnWndProc = window_proc;
		window_class.hInstance = instance;
		window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		window_class.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
		window_class.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
		window_class.hbrBackground = CreateSolidBrush(color_background);
		window_class.lpszClassName = window_class_name;
		if (!RegisterClassExW(&window_class))
		{
			if (instance_mutex)
				CloseHandle(instance_mutex);
			return 1;
		}

		auto state = std::make_unique<app_state>();
		state->instance = instance;
		state->module_directory = executable_directory();

		RECT desired{0, 0, 900, 650};
		AdjustWindowRectEx(&desired,
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			FALSE, WS_EX_APPWINDOW);
		const int width = desired.right - desired.left;
		const int height = desired.bottom - desired.top;
		const int x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		const int y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		HWND window = CreateWindowExW(
			WS_EX_APPWINDOW,
			window_class_name,
			L"THAutomation — 一键注入启动器",
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			x, y, width, height,
			nullptr, nullptr, instance, state.get());
		if (!window)
		{
			if (instance_mutex)
				CloseHandle(instance_mutex);
			return 1;
		}

		ShowWindow(window, show_command);
		UpdateWindow(window);

		MSG message{};
		while (GetMessageW(&message, nullptr, 0, 0) > 0)
		{
			if (!IsDialogMessageW(window, &message))
			{
				TranslateMessage(&message);
				DispatchMessageW(&message);
			}
		}

		state.reset();
		if (instance_mutex)
			CloseHandle(instance_mutex);
		return static_cast<int>(message.wParam);
	}
}
