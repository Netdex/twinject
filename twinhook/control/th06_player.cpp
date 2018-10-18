#include "stdafx.h"
#include "th06_player.h"
#include "config/th_config.h"
#include "hook/th_di8_hook.h"


void th06_player::onInit()
{
	th_player::onInit();
}

void th06_player::onTick()
{
	th_player::onTick();
}

void th06_player::onBeginTick()
{
	th_player::onBeginTick();
}

void th06_player::onAfterTick()
{
	th_player::onAfterTick();
}

void th06_player::draw(IDirect3DDevice9* d3dDev)
{
	th_player::draw(d3dDev);
}

void th06_player::handleInput(const BYTE diKeys[256], const BYTE press[256])
{
	th_player::handleInput(diKeys, press);
}

void th06_player::onEnableChanged(bool enable)
{
	th_player::onEnableChanged(enable);
}

entity th06_player::getPlayerEntity()
{
	return {};
}
