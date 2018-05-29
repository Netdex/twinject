#pragma once
#include "th_player.h"

class th07_player : public th_player
{
public:

	th07_player() : th_player(gs_addr{ (uint8_t*)0x4BDCA0, (uint8_t*)0x4B9E50 }) {}
	~th07_player() {}

	void onInit() override;
	void onTick() override;
	void onBeginTick() override;
	void onAfterTick() override;
	void draw(IDirect3DDevice9* d3dDev) override;

	void handleInput(const BYTE diKeys[256], const BYTE press[256]) override;
	void onEnableChanged(bool enable) override;
};
