#include "stdafx.h"
#include "wfth15_player.h"
#include "config/th_config.h"
#include "hook/th_di8_hook.h"

#include "WireframeTouhou/th15.h"
#include "WireframeTouhou/inject.h"

extern InjecteeLdrCtx *g_injectee_ldr;

void wfth15_player::onInit()
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

	ASSERT(("wfth could not hook", th15_reader.Hook() == 0));
}

void wfth15_player::onTick()
{
	th_player::onTick();
}

void wfth15_player::onBeginTick()
{
	th_player::onBeginTick();
	this->doBulletPoll();
	this->doEnemyPoll();
	this->doPowerupPoll();
	this->doLaserPoll();
}

void wfth15_player::onAfterTick()
{
	th_player::onAfterTick();
}

void wfth15_player::draw(IDirect3DDevice9* d3dDev)
{
	th_player::draw(d3dDev);
}

void wfth15_player::handleInput(const BYTE diKeys[256], const BYTE press[256])
{
	th_player::handleInput(diKeys, press);
}

void wfth15_player::onEnableChanged(bool enable)
{
	th_player::onEnableChanged(enable);
}

void wfth15_player::doBulletPoll()
{
	for (size_t i = 0; i != state.bullet.size(); i++)
	{
		auto hb = state.bullet[i];
		circle a{
			vec2(hb.pos.x + th_param.GAME_WIDTH / 2, hb.pos.y),
			vec2(),		// TODO wfth does not support velocities?
			hb.size.x / 2
		};
		bullet b{ a };
		bullets.push_back(b);
	}
}

void wfth15_player::doEnemyPoll()
{
	//for(size_t i = 0; i != state.)
}

void wfth15_player::doPowerupPoll()
{

}

void wfth15_player::doLaserPoll()
{

}

player wfth15_player::getPlayerEntity()
{
	auto pl = state.player;
	circle a{
		vec2(pl.pos.x + th_param.GAME_WIDTH / 2, pl.pos.y),
		vec2(),
		pl.size.x / 2
	};
	return player{ a };
}


