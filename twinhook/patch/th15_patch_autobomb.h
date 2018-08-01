#pragma once
#include "th_patch.h"

class th15_patch_autobomb : public th_patch
{
public:
	th15_patch_autobomb() : th_patch({
		th_patch_def((void*)0x00454CC9, "\xC6", 1)
	}){}
};
