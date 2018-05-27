#pragma once
#include "th_patch.h"

class th15_patch_autobomb : th_patch
{
public:
	void patch() override;
	void unpatch() override;
};
