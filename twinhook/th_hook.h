#pragma once
#include "th_player.h"

class th_hook
{
protected:
	th_player *player;
public:
	th_hook(th_player *player) : player(player) {}
	virtual ~th_hook() = default;

	virtual void hook() = 0;
};
