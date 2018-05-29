#pragma once
#include "th_player.h"


class th08_player : public th_player
{
public:
	th08_player() : th_player(gs_addr{ (uint8_t*)0x017D6110, (uint8_t*)0x164D52C }) {}
	~th08_player() = default;

	void onInit() override;
	void onTick() override;
	void onBeginTick() override;
	void onAfterTick() override;
	void draw(IDirect3DDevice9* d3dDev) override;

	void handleInput(const BYTE diKeys[256], const BYTE press[256]) override;
	void onEnableChanged(bool enable) override;
};
