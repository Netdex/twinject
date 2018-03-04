#pragma once
#include "th_player.h"
#include "vec2.h"

class th07_player : public th_player
{
public:

	th07_player() {}
	~th07_player() {}

	void on_init() override;
	void on_tick() override;
	void on_begin_tick() override;
	void on_after_tick() override;
	void draw(IDirect3DDevice9* d3dDev) override;

	void handle_input(BYTE diKeys[256]) override;
	void on_enable_changed(bool enable) override;

private:
	entity get_plyr_cz() override;
};
