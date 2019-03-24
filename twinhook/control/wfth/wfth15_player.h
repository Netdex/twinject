#pragma once
#include "control/th_player.h"
#include "WireframeTouhou/th15.h"
#include "WireframeTouhou/remote_app.h"

class wfth15_player : public th_player, public RemoteApp
{
	th::Th15Reader th15_reader;
	th::GameState state;
public:
	wfth15_player() : th_player(gs_addr{ (uint8_t*)0x004E9BB8,(uint8_t*)0x4E6F28 }) {}
	~wfth15_player() = default;

	void onInit() override;
	void onTick() override;
	void onBeginTick() override;
	void onAfterTick() override;
	void draw(IDirect3DDevice9* d3dDev) override;

	void handleInput(const BYTE diKeys[256], const BYTE press[256]) override;
	void onEnableChanged(bool enable) override;

	th::GameState* GetState() override { return &state; }
	th::StateReader* GetReader() override { return &th15_reader; }

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
