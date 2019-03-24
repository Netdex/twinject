#include "stdafx.h"
#include "wfth11_player.h"
#include "config/th_config.h"
#include "hook/th_di8_hook.h"

#include "WireframeTouhou/th15.h"
#include "WireframeTouhou/inject.h"

extern InjecteeLdrCtx *g_injectee_ldr;

void wfth11_player::onInit()
{
	th_player::onInit();

	LOG("Configuring WFTH to inject without RemoteApp");
	// Define this app as active RemoteApp to detach from injection routine
	RemoteApp::set_inst(this);

	// Fake RunConfig to satisfy requirement of hook
	g_injectee_ldr = new InjecteeLdrCtx();
	RunConfig *rc = new RunConfig();
	g_injectee_ldr->injectee.data = rc;
	rc->hook = true;

	ASSERT(("wfth could not hook", th11_reader.Hook() == 0));
}

void wfth11_player::onTick()
{
	th_player::onTick();
}

void wfth11_player::onBeginTick()
{
	th_player::onBeginTick();
	this->doBulletPoll();
	this->doEnemyPoll();
	this->doPowerupPoll();
	this->doLaserPoll();
}

void wfth11_player::onAfterTick()
{
	th_player::onAfterTick();
}

void wfth11_player::draw(IDirect3DDevice9* d3dDev)
{
	th_player::draw(d3dDev);
}

void wfth11_player::handleInput(const BYTE diKeys[256], const BYTE press[256])
{
	th_player::handleInput(diKeys, press);
}

void wfth11_player::onEnableChanged(bool enable)
{
	th_player::onEnableChanged(enable);
}

void wfth11_player::doBulletPoll()
{
	for (size_t i = 0; i != state.bullet.size(); i++)
	{
		auto hb = state.bullet[i];
		aabb a{
			vec2(hb.pos.x + th_param.GAME_WIDTH / 2 - hb.size.x / 2, hb.pos.y - hb.size.y / 2),
			vec2(hb.vel.x, hb.vel.y),		// TODO wfth does not support velocities?
			vec2(hb.size.x, hb.size.y)
		};
		bullet b{ a };
		bullets.push_back(b);
	}
}

void wfth11_player::doEnemyPoll()
{
	for (size_t i = 0; i != state.enemy.size(); i++)
	{
		auto hb = state.enemy[i];
		circle a{
			vec2(hb.pos.x + th_param.GAME_WIDTH / 2, hb.pos.y),
			vec2(hb.vel.x, hb.vel.y),		// TODO wfth does not support velocities?
			hb.size.x / 2
		};
		enemy b{ a };
		enemies.push_back(b);
	}
}

void wfth11_player::doPowerupPoll()
{
	
}

void wfth11_player::doLaserPoll()
{

}

player wfth11_player::getPlayerEntity()
{
	/*
	 * (PLYR BASE PTR)
	 * [0x87C]	?
	 * [0x8]	POS (VEC2)
	 * [0x48]	?
	 * [0xC]	BOUND TOP LEFT (VEC3)
	 * [0xC]	BOUND BOTTOM RIGHT (VEC3)
	 */
	PBYTE *PlayerPtrAddr = (PBYTE*)gs_ptr.plyr_pos;
	if (*PlayerPtrAddr) {
		PBYTE plyrAddr = *PlayerPtrAddr;

		float w = (*(float*)(plyrAddr + 0x87C + 0x8 + 0x48 + 0xC)
			- *(float*)(plyrAddr + 0x87C + 0x8 + 0x48)) * 2;
		float h = (*(float*)(plyrAddr + 0x87C + 0x8 + 0x48 + 0xC + 4)
			- *(float*)(plyrAddr + 0x87C + 0x8 + 0x48 + 4)) * 2;
		vec2 sz(w, h);
		aabb e = {
			vec2(
				*(float*)(plyrAddr + 0x87C) + th_param.GAME_WIDTH / 2,
				*(float*)(plyrAddr + 0x87C + 4)) - sz / 2,
			vec2(),
			sz,
		};
		return e;
	}
	return player{ aabb() };
}


