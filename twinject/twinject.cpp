#include "launcher.h"

#include <windows.h>

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command)
{
	return thautomation::run_launcher(instance, show_command);
}
