#pragma once
#include "th_hook.h"
#include "th_player.h"

class th_d3d9_hook : th_hook
{
	th_player &player;
protected:
	~th_d3d9_hook() = default;
public:
	explicit th_d3d9_hook(th_player &player);

	void hook() override;
};
