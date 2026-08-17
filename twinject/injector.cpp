#include "injector.h"

#include <algorithm>
#include <cwctype>
#include <fstream>
#include <sstream>
#include <system_error>
#include <vector>

#include <tlhelp32.h>
#include <detours.h>

namespace fs = std::filesystem;

namespace thautomation
{
	namespace
	{
		const std::vector<game_definition> games{
			{L"th06", L"TH06  东方红魔乡", {L"th06.exe", L"th06e.exe"}, true, false},
			{L"th07", L"TH07  东方妖妖梦", {L"th07.exe", L"th07e.exe"}, true, false},
			{L"th08", L"TH08  东方永夜抄", {L"th08.exe", L"th08e.exe"}, true, false},
			{L"th09", L"TH09  东方花映塚（实验性）", {L"th09.exe", L"th09e.exe"}, false, true},
			{L"th10", L"TH10  东方风神录", {L"th10.exe", L"th10e.exe"}, false, false},
			{L"th11", L"TH11  东方地灵殿", {L"th11.exe", L"th11e.exe"}, false, false},
			{L"th15", L"TH15  东方绀珠传", {L"th15.exe", L"th15e.exe"}, false, false},
		};

		std::wstring lowercase(std::wstring value)
		{
			std::transform(value.begin(), value.end(), value.begin(),
				[](wchar_t ch) { return static_cast<wchar_t>(std::towlower(ch)); });
			return value;
		}

		bool path_exists(const fs::path& path)
		{
			std::error_code error;
			return fs::is_regular_file(path, error);
		}

		bool is_pe32_file(const fs::path& path, std::wstring& error_message)
		{
			std::ifstream file(path, std::ios::binary);
			if (!file)
			{
				error_message = L"无法读取文件。";
				return false;
			}

			IMAGE_DOS_HEADER dos_header{};
			file.read(reinterpret_cast<char*>(&dos_header), sizeof(dos_header));
			if (!file || dos_header.e_magic != IMAGE_DOS_SIGNATURE || dos_header.e_lfanew <= 0)
			{
				error_message = L"不是有效的 Windows 可执行文件。";
				return false;
			}

			file.seekg(dos_header.e_lfanew, std::ios::beg);
			DWORD signature = 0;
			IMAGE_FILE_HEADER file_header{};
			file.read(reinterpret_cast<char*>(&signature), sizeof(signature));
			file.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));
			if (!file || signature != IMAGE_NT_SIGNATURE)
			{
				error_message = L"PE 文件头无效。";
				return false;
			}

			if (file_header.Machine != IMAGE_FILE_MACHINE_I386)
			{
				error_message = L"仅支持 32 位（x86）游戏和 DLL。";
				return false;
			}
			return true;
		}

		bool runtime_available()
		{
			wchar_t found[MAX_PATH]{};
			return SearchPathW(nullptr, L"d3dx9_43.dll", nullptr, MAX_PATH, found, nullptr) > 0;
		}

		bool process_is_running(const fs::path& executable_path)
		{
			const auto expected = lowercase(executable_path.filename().wstring());
			HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (snapshot == INVALID_HANDLE_VALUE)
				return false;

			PROCESSENTRY32W entry{};
			entry.dwSize = sizeof(entry);
			bool found = false;
			if (Process32FirstW(snapshot, &entry))
			{
				do
				{
					if (lowercase(entry.szExeFile) == expected)
					{
						found = true;
						break;
					}
				} while (Process32NextW(snapshot, &entry));
			}
			CloseHandle(snapshot);
			return found;
		}

		std::wstring quote_argument(const std::wstring& value)
		{
			if (value.find_first_of(L" \t\"") == std::wstring::npos)
				return value;

			std::wstring result = L"\"";
			size_t backslashes = 0;
			for (const wchar_t ch : value)
			{
				if (ch == L'\\')
				{
					++backslashes;
					continue;
				}
				if (ch == L'"')
				{
					result.append(backslashes * 2 + 1, L'\\');
					result.push_back(L'"');
					backslashes = 0;
					continue;
				}
				result.append(backslashes, L'\\');
				backslashes = 0;
				result.push_back(ch);
			}
			result.append(backslashes * 2, L'\\');
			result.push_back(L'"');
			return result;
		}

		bool to_detours_path(const fs::path& path, std::string& output)
		{
			auto convert = [](const std::wstring& input, std::string& converted) {
				BOOL used_default = FALSE;
				const int size = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
					input.c_str(), -1, nullptr, 0, nullptr, &used_default);
				if (size <= 0 || used_default)
					return false;
				converted.resize(static_cast<size_t>(size));
				const bool success = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
					input.c_str(), -1, converted.data(), size, nullptr, &used_default) > 0 &&
					!used_default;
				if (success && !converted.empty() && converted.back() == '\0')
					converted.pop_back();
				return success;
			};

			if (convert(path.wstring(), output))
				return true;

			std::vector<wchar_t> short_path(32768);
			const DWORD length = GetShortPathNameW(path.c_str(), short_path.data(),
				static_cast<DWORD>(short_path.size()));
			if (length == 0 || length >= short_path.size())
				return false;
			return convert(short_path.data(), output);
		}

		bool copy_legacy_renderer(const launch_request& request, std::wstring& error_message)
		{
			if (!request.game->requires_legacy_renderer || !request.install_legacy_renderer)
				return true;

			const fs::path game_directory = request.executable_path.parent_path();
			const fs::path destination_dll = game_directory / L"d3d8.dll";
			if (path_exists(destination_dll))
				return true;

			const fs::path source_dll = request.legacy_renderer_directory / L"d3d8.dll";
			const fs::path source_config = request.legacy_renderer_directory / L"enbconvertor.ini";
			if (!path_exists(source_dll))
			{
				error_message = L"旧版 DirectX 兼容层未包含在发布包中。请重新解压完整发布包。";
				return false;
			}

			std::error_code error;
			if (path_exists(source_config) && !path_exists(game_directory / L"enbconvertor.ini"))
			{
				fs::copy_file(source_config, game_directory / L"enbconvertor.ini",
					fs::copy_options::none, error);
				if (error)
				{
					error_message = L"无法写入游戏目录中的 enbconvertor.ini：\n" +
						format_windows_error(static_cast<DWORD>(error.value()));
					return false;
				}
			}

			error.clear();
			fs::copy_file(source_dll, destination_dll, fs::copy_options::none, error);
			if (error)
			{
				error_message = L"无法把 d3d8.dll 安装到游戏目录。请检查目录权限或以管理员身份运行。\n\n" +
					format_windows_error(static_cast<DWORD>(error.value()));
				return false;
			}
			return true;
		}

		class environment_guard
		{
		public:
			environment_guard(const wchar_t* name, const std::wstring& value) : name_(name)
			{
				SetLastError(ERROR_SUCCESS);
				const DWORD length = GetEnvironmentVariableW(name_.c_str(), nullptr, 0);
				existed_ = length > 0 || GetLastError() != ERROR_ENVVAR_NOT_FOUND;
				if (length > 0)
				{
					old_value_.resize(length);
					GetEnvironmentVariableW(name_.c_str(), old_value_.data(), length);
					if (!old_value_.empty() && old_value_.back() == L'\0')
						old_value_.pop_back();
				}
				SetEnvironmentVariableW(name_.c_str(), value.c_str());
			}

			~environment_guard()
			{
				SetEnvironmentVariableW(name_.c_str(), existed_ ? old_value_.c_str() : nullptr);
			}

		private:
			std::wstring name_;
			std::wstring old_value_;
			bool existed_ = false;
		};
	}

	const std::vector<game_definition>& supported_games()
	{
		return games;
	}

	const game_definition* find_game_by_id(const std::wstring& id)
	{
		const auto lowered = lowercase(id);
		for (const auto& game : games)
			if (lowercase(game.id) == lowered)
				return &game;
		return nullptr;
	}

	const game_definition* find_game_for_executable(const fs::path& path)
	{
		const auto filename = lowercase(path.filename().wstring());
		for (const auto& game : games)
			for (const auto& expected : game.executable_names)
				if (lowercase(expected) == filename)
					return &game;
		return nullptr;
	}

	validation_report validate_launch_request(const launch_request& request)
	{
		validation_report report{};
		if (!request.game)
		{
			report.summary = L"请选择游戏。";
			return report;
		}
		if (request.executable_path.empty())
		{
			report.summary = L"请选择游戏 EXE。";
			return report;
		}
		if (!path_exists(request.executable_path))
		{
			report.summary = L"游戏 EXE 不存在，请重新选择。";
			return report;
		}

		std::wstring pe_error;
		if (!is_pe32_file(request.executable_path, pe_error))
		{
			report.summary = L"游戏 EXE 校验失败：" + pe_error;
			return report;
		}
		if (!path_exists(request.hook_dll_path))
		{
			report.summary = L"找不到 twinhook.dll，请重新解压完整发布包。";
			return report;
		}
		if (!is_pe32_file(request.hook_dll_path, pe_error))
		{
			report.summary = L"twinhook.dll 校验失败：" + pe_error;
			return report;
		}
		if (!runtime_available())
		{
			report.summary = L"缺少 DirectX June 2010 运行库（d3dx9_43.dll）。";
			return report;
		}
		if (process_is_running(request.executable_path))
		{
			report.summary = L"游戏已经在运行。注入必须发生在启动前，请先关闭游戏。";
			return report;
		}

		const auto detected = find_game_for_executable(request.executable_path);
		if (detected && detected->id != request.game->id)
			report.warnings.push_back(L"EXE 名称看起来属于 " + detected->display_name + L"，请确认游戏选择。");
		else if (!detected)
			report.warnings.push_back(L"这是非标准 EXE 文件名；补丁版可能不兼容当前内存地址。");

		if (request.game->experimental)
			report.warnings.push_back(L"TH09 当前仅提供部分功能，敌人、道具与激光支持仍不完整。");

		if (request.game->requires_legacy_renderer && request.install_legacy_renderer)
		{
			const fs::path installed = request.executable_path.parent_path() / L"d3d8.dll";
			const fs::path bundled = request.legacy_renderer_directory / L"d3d8.dll";
			if (!path_exists(installed) && !path_exists(bundled))
			{
				report.summary = L"缺少旧版 DirectX 兼容层 d3d8.dll，请重新解压完整发布包。";
				return report;
			}
		}
		else if (request.game->requires_legacy_renderer)
		{
			report.warnings.push_back(
				L"已关闭旧版 DirectX 兼容层；若游戏目录没有可用的 d3d8.dll，图形界面将无法挂接。");
		}

		report.ready = true;
		report.summary = report.warnings.empty()
			? L"准备就绪，可以一键注入并启动。"
			: L"可以启动，但请留意兼容性提示。";
		return report;
	}

	launch_result launch_injected(const launch_request& request)
	{
		launch_result result{};
		const validation_report validation = validate_launch_request(request);
		if (!validation.ready)
		{
			result.error_code = ERROR_INVALID_PARAMETER;
			result.message = validation.summary;
			return result;
		}

		if (!copy_legacy_renderer(request, result.message))
		{
			result.error_code = ERROR_ACCESS_DENIED;
			return result;
		}

		std::string hook_dll_ansi;
		if (!to_detours_path(fs::absolute(request.hook_dll_path), hook_dll_ansi))
		{
			result.error_code = ERROR_NO_UNICODE_TRANSLATION;
			result.message = L"twinhook.dll 路径包含当前系统无法转换的字符。请把发布包移动到较短的英文路径后重试。";
			return result;
		}

		const fs::path executable = fs::absolute(request.executable_path);
		const fs::path working_directory = executable.parent_path();
		std::wstring command_line = quote_argument(executable.wstring());
		if (!request.arguments.empty())
		{
			command_line.push_back(L' ');
			command_line += request.arguments;
		}
		std::vector<wchar_t> mutable_command(command_line.begin(), command_line.end());
		mutable_command.push_back(L'\0');

		STARTUPINFOW startup_info{};
		PROCESS_INFORMATION process_info{};
		startup_info.cb = sizeof(startup_info);

		environment_guard game_environment(L"th", request.game->id);
		SetDllDirectoryW(request.hook_dll_path.parent_path().c_str());

		const BOOL created = DetourCreateProcessWithDllW(
			executable.c_str(),
			mutable_command.data(),
			nullptr,
			nullptr,
			FALSE,
			CREATE_DEFAULT_ERROR_MODE,
			nullptr,
			working_directory.c_str(),
			&startup_info,
			&process_info,
			hook_dll_ansi.c_str(),
			nullptr);
		const DWORD create_error = created ? ERROR_SUCCESS : GetLastError();
		SetDllDirectoryW(nullptr);

		if (!created)
		{
			result.error_code = create_error;
			result.message = L"注入启动失败：\n" + format_windows_error(create_error);
			return result;
		}

		CloseHandle(process_info.hThread);
		result.success = true;
		result.process_id = process_info.dwProcessId;
		result.process_handle = process_info.hProcess;
		result.message = L"注入成功，游戏已启动。";
		return result;
	}

	std::wstring format_windows_error(const DWORD error_code)
	{
		wchar_t* buffer = nullptr;
		const DWORD length = FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<wchar_t*>(&buffer),
			0,
			nullptr);

		std::wstring result = length && buffer ? std::wstring(buffer, length) : L"未知错误";
		if (buffer)
			LocalFree(buffer);
		while (!result.empty() && (result.back() == L'\r' || result.back() == L'\n' || result.back() == L' '))
			result.pop_back();

		std::wstringstream stream;
		stream << result << L"（错误 " << error_code << L"）";
		return stream.str();
	}
}
