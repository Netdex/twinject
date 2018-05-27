#pragma once
#include "../control/th_player.h"

class th_hook
{
public:
	th_player *player;

	th_hook(th_player *player) : player(player) {}
	virtual ~th_hook() = default;
};
