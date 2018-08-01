#pragma once
#include "th_patch.h"

class th10_patch_autobomb : public th_patch
{
public:
	th10_patch_autobomb() : th_patch({
		th_patch_def((void*)0x00425C13, "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 13)
	}){}
};
