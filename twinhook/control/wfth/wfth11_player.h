#pragma once
#include "control/th_player.h"
#include "WireframeTouhou/remote_app.h"
#include "WireframeTouhou/th11.h"

class wfth11_player : public th_player, public RemoteApp
{
	th::Th11Reader th11_reader;
	th::GameState state;
public:
	// TODO populate game-specific addresses
	wfth11_player() : th_player(gs_addr{ (uint8_t*) 0x4A8EB4,(uint8_t*)0x4C93C0 }) {}
	~wfth11_player() = default;

	void onInit() override;
	void onTick() override;
	void onBeginTick() override;
	void onAfterTick() override;
	void draw(IDirect3DDevice9* d3dDev) override;

	void handleInput(const BYTE diKeys[256], const BYTE press[256]) override;
	void onEnableChanged(bool enable) override;

	th::GameState* GetState() override { return &state; }
	th::StateReader* GetReader() override { return &th11_reader; }

	/* UNUSED */
	int Init() override { return 0; }
	int Run() override { return 0; }
	void OnPreTick() override {}
	void OnPostTick() override {}
	void OnPrePaint() override {}
	void OnPostPaint() override {}
private:
	void doBulletPoll();
	void doEnemyPoll();
	void doPowerupPoll();
	void doLaserPoll();

	player getPlayerEntity() override;
};
