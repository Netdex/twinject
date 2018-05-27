#pragma once

#include "th_hook.h"
#include "../control/th10_player.h"


class th10_bullet_proc_hook : public th_hook
{
	static th10_bullet_proc_hook *instance;
public:
	th10_bullet_proc_hook(th10_player *player) : th_hook(player) {}

	static void bind(th10_player *player);
	static th10_bullet_proc_hook *inst();
};
