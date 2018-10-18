#pragma once
#pragma once
#include "th_player.h"

class th06_player : public th_player
{
public:
	th06_player() : th_player(gs_addr{ (uint8_t*)0x477834,(uint8_t*)0x474E5C }) {}
	~th06_player() = default;

	void onInit() override;
	void onTick() override;
	void onBeginTick() override;
	void onAfterTick() override;
	void draw(IDirect3DDevice9* d3dDev) override;

	void handleInput(const BYTE diKeys[256], const BYTE press[256]) override;
	void onEnableChanged(bool enable) override;

private:
	entity getPlayerEntity() override;
};
