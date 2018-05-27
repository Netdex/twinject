#pragma once
#include "th_player.h"

class th07_player : public th_player
{
public:

	th07_player() : th_player(gs_addr{ (uint8_t*)0x4BDCA0, (uint8_t*)0x4B9E50 }) {}
	~th07_player() {}

	void on_init() override;
	void on_tick() override;
	void on_begin_tick() override;
	void on_after_tick() override;
	void draw(IDirect3DDevice9* d3dDev) override;

	void handle_input(const BYTE diKeys[256], const BYTE press[256]) override;
	void on_enable_changed(bool enable) override;
};
