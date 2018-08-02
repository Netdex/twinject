#pragma once
#include "th_patch.h"

/*
 * th11 seems to have some issues initializing DirectInput, which causes the bot to fail 
 * to send inputs to the game correctly.
 * 
 * This patch is a translation into "twinject-like" patch notation from the thcrap base_tsa 
 * package, licensed under the Unlicense.
 * https://github.com/thpatch/thcrap-tsa/blob/master/base_tsa/th11.js
 * Thank you!
 */
class th11_patch_dinput_fix : public th_patch
{
public:
	/* NOTE: assuming base address is always 0x400000, don't know if that's 
	 * always true though
	 */
	th11_patch_dinput_fix() : th_patch({
		th_patch_def((void*)0x446ADD, "\x50\x8b\x18", 3),
		th_patch_def((void*)0x446D20, "\x58\x31\xdb\xe9\x88\x0d\x00\x00", 8),
		th_patch_def((void*)0x4456C3, "\xe8\x15\x14\x00\x00", 5),
		th_patch_def((void*)0x445737, "\xa1\x88\x3d\x4c\x00\x85\xc0\x0f\x84", 9),
	}) {}
};
