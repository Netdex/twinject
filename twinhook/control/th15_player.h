#pragma once
#include "th_player.h"


class th15_player : public th_player
{
public:
	th15_player() : th_player(gs_addr{ (uint8_t*)0x004E9BB8,(uint8_t*)0x4E6F28 }) {}
	~th15_player() = default;

	void on_init() override;
	void on_tick() override;
	void on_begin_tick() override;
	void on_after_tick() override;
	void draw(IDirect3DDevice9* d3dDev) override;

	void handle_input(const BYTE diKeys[256], const BYTE press[256]) override;
	void on_enable_changed(bool enable) override;

private:
	/**
	 * Note: the size for players in th15 is radius, not diameter.
	 */
	entity get_plyr_ent() override;
};
