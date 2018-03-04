#pragma once
#include "th_player.h"
#include "vec2.h"



class th15_player : public th_player
{
public:
	th15_player() {}
	~th15_player() = default;

	void on_init() override;
	void on_tick() override;
	void on_begin_tick() override;
	void on_after_tick() override;
	void draw(IDirect3DDevice9* d3dDev) override;

	void handle_input(BYTE diKeys[256]) override;
	void on_enable_changed(bool enable) override;

private:
	/**
	 * Note: the size for players in th15 is radius, not diameter.
	 */
	entity get_plyr_cz() override;
};
