#pragma once
#include "th_player.h"
#include "vec2.h"



class th08_player : public th_player
{
public:
	th08_player() : th_player(gs_addr{ (uint8_t*)0x017D6110, (uint8_t*)0x164D52C }) {}
	~th08_player() = default;

	void on_init() override;
	void on_tick() override;
	void on_begin_tick() override;
	void on_after_tick() override;
	void draw(IDirect3DDevice9* d3dDev) override;

	void handle_input(const BYTE diKeys[256], const BYTE press[256]) override;
	void on_enable_changed(bool enable) override;
};
