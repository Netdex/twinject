#pragma once
#include "th_patch.h"

class th15_patch_autobomb : public th_patch
{
public:
	void patch() override;
	void unpatch() override;
};
