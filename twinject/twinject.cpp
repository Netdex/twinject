#include <iostream>
#include <filesystem>

#include <windows.h>
#include <detours.h>

#include <cpptoml.h>

#include "debugger.h"

STARTUPINFOA si;
PROCESS_INFORMATION pi;

// If you use an external debugger, this line must be commented out. 
// Debugging messages will not be received.
#define DEBUGGER				

int main(const int argc, const char* argv[])
{
	std::cout << "TWINJECT - Touhou Windows Injector (netdex)\n\
===========================================" << std::endl;

	std::shared_ptr<cpptoml::table> config;
	try {
		config = cpptoml::parse_file("twinject.toml");
	}
	catch (cpptoml::parse_exception& e)
	{
		std::cerr << "twinject.toml contains a syntax error:" << std::endl;
		std::cerr << e.what() << std::endl;
		return 1;
	}

	bool debug_mode = *config->get_as<bool>("debug");
	std::string env = *config->get_as<std::string>("env");
	std::string dll = *config->get_as<std::string>("dll");

	std::filesystem::path twinhook_dll_path;
	std::filesystem::path bin_path;
	std::filesystem::path cur_path;

	if (debug_mode)
	{
		std::cout << "WARNING: debug=true, so injector will use hardcoded paths!" << std::endl;
		auto env_table = config->get_table(env);

		cur_path = *env_table->get_as<std::string>("path");
		std::string bin = *env_table->get_as<std::string>("bin");
		bin_path = cur_path / bin;
#ifdef DEBUG
		twinhook_dll_path = *config->get_as<std::string>("twinhook_path_dbg");
#else
		twinhook_dll_path = *config->get_as<std::string>("twinhook_path_rls");
#endif
	}
	else
	{
		std::string bin = *config->get_as<std::string>("bin");
		cur_path = std::filesystem::current_path();
		bin_path = cur_path / bin;
		twinhook_dll_path = cur_path / dll;
	}

#ifdef DEBUG
	std::cout << "WARNING: Injector was compiled in debug mode!" << std::endl;
#endif

	SetEnvironmentVariable("th", env.c_str());

#ifndef DEBUGGER
	std::cout << "WARNING: Debugger disabled upon compile-time! No debug messages will appear!" << std::endl;
#endif
	std::cout << "twinject: Adding path to DLL search path: " << twinhook_dll_path << std::endl;
	SetDllDirectory(twinhook_dll_path.string().c_str());

	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);
	if (DetourCreateProcessWithDll(bin_path.string().c_str(),
		NULL, NULL, NULL, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE
#ifdef DEBUGGER
		| DEBUG_PROCESS
#endif
		, NULL,
		cur_path.string().c_str(),
		&si, &pi, twinhook_dll_path.string().c_str(), NULL))
	{
		std::cout << "twinject: Injection OK" << std::endl;
	}
	else
	{
		std::cout << "twinject: Injection FAIL" << std::endl;
		return 1;
	}

	DEBUG_EVENT debugEv;
	EnterDebugLoop(&debugEv);

	return 0;
}

