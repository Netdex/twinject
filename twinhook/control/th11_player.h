#pragma once
#include "th_player.h"

class th11_player : public th_player
{
public:
	// TODO populate game-specific addresses
	th11_player() : th_player(gs_addr{ (uint8_t*) 0x4A8EB4,(uint8_t*)0x4C93C0 }) {}
	~th11_player() = default;

	void onInit() override;
	void onTick() override;
	void onBeginTick() override;
	void onAfterTick() override;
	void draw(IDirect3DDevice9* d3dDev) override;

	void handleInput(const BYTE diKeys[256], const BYTE press[256]) override;
	void onEnableChanged(bool enable) override;

private:
	void doBulletPoll();
	void doEnemyPoll();
	void doPowerupPoll();
	void doLaserPoll();

	entity getPlayerEntity() override;
};
