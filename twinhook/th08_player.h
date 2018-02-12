#pragma once
#include "th_player.h"
#include "th_d3d9_hook.h"
#include "th_di8_hook.h"
#include "th08_bullet_process_hook.h"

class th08_player : public th_player
{
	th_d3d9_hook d3d9_hook;
	th_di8_hook di8_hook;
	th08_bullet_process_hook bp_hook;
public:
	th08_player();
	~th08_player() = default;


	void on_init() override;
	void hook() override;
	void on_tick() override;
};
