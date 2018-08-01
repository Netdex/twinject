#pragma once
#include "th_patch.h"

class th08_patch_autobomb : public th_patch
{
public:
	th08_patch_autobomb() : th_patch({
		th_patch_def((void*)0x0044CC18, "\xFF\x89", 2),
		th_patch_def((void*)0x0044CC21, "\x66\xC7\x05\x28\xD5\x64\x01\x02", 8)
	}) {}
};
