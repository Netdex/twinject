#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <windows.h>

namespace thautomation
{
	struct game_definition
	{
		std::wstring id;
		std::wstring display_name;
		std::vector<std::wstring> executable_names;
		bool requires_legacy_renderer = false;
		bool experimental = false;
	};

	struct launch_request
	{
		const game_definition* game = nullptr;
		std::filesystem::path executable_path;
		std::filesystem::path hook_dll_path;
		std::filesystem::path legacy_renderer_directory;
		std::wstring arguments;
		bool install_legacy_renderer = true;
	};

	struct validation_report
	{
		bool ready = false;
		std::wstring summary;
		std::vector<std::wstring> warnings;
	};

	struct launch_result
	{
		bool success = false;
		DWORD error_code = ERROR_SUCCESS;
		DWORD process_id = 0;
		HANDLE process_handle = nullptr;
		std::wstring message;
	};

	const std::vector<game_definition>& supported_games();
	const game_definition* find_game_by_id(const std::wstring& id);
	const game_definition* find_game_for_executable(const std::filesystem::path& path);

	validation_report validate_launch_request(const launch_request& request);
	launch_result launch_injected(const launch_request& request);

	std::wstring format_windows_error(DWORD error_code);
}
