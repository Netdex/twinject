#include "stdafx.h"
#include "th08_patch_autobomb.h"

static char *sc_original_0x0044CC18 = "\x8B\x91";
static char *sc_original_0x0044CC21 = "\x8B\x45\xF8\x89\x90\x68\x2A\x0E";

static char *sc_patch_0x0044CC18 = "\xFF\x89";
static char *sc_patch_0x0044CC21 = "\x66\xC7\x05\x28\xD5\x64\x01\x02";

void th08_patch_autobomb::patch()
{
	memcpy((void*)0x0044CC18, sc_patch_0x0044CC18, 2);
	memcpy((void*)0x0044CC21, sc_patch_0x0044CC21, 8);
}

void th08_patch_autobomb::unpatch()
{
	memcpy((void*)0x0044CC18, sc_original_0x0044CC18, 2);
	memcpy((void*)0x0044CC21, sc_original_0x0044CC21, 8);
}
