#include "config.h"

#include <string.h>
#include <windows.h>
#include <shlwapi.h>

int handler(void* user, const char* section, const char* name,
	const char* value)
{
	configuration* pconfig = (configuration*)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
	if (MATCH("twinject", "exename")) {
		pconfig->exename = StrDup(value);
	}
	else if (MATCH("twinject", "dllname")) {
		pconfig->dllname = StrDup(value);
	}
	else {
		return 0;
	}
	return 1;
}
