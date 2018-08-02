#include "stdafx.h"
#include "th11_player.h"
#include "config/th_config.h"
#include "hook/th_di8_hook.h"


void th11_player::onInit()
{
	th_player::onInit();
}

void th11_player::onTick()
{
	th_player::onTick();
}

void th11_player::onBeginTick()
{
	th_player::onBeginTick();
	this->doBulletPoll();
	this->doEnemyPoll();
	this->doPowerupPoll();
	this->doLaserPoll();
}

void th11_player::onAfterTick()
{
	th_player::onAfterTick();
}

void th11_player::draw(IDirect3DDevice9* d3dDev)
{
	th_player::draw(d3dDev);
}

void th11_player::handleInput(const BYTE diKeys[256], const BYTE press[256])
{
	th_player::handleInput(diKeys, press);
}

void th11_player::onEnableChanged(bool enable)
{
	th_player::onEnableChanged(enable);
}

void th11_player::doBulletPoll()
{
	int *dword_4A8D68 = (int*)0x4A8D68;

	if (!*(int*)dword_4A8D68)
		return;

	char *pBase = (char*)*dword_4A8D68 + 100;
	int i = 2000;
	do
	{
		if (*pBase & 1)
		{
			if (*((WORD *)pBase + 601) == 1) {
				float w = *((float *)pBase + 279);
				float x = *((float *)pBase + 271);
				float h = *((float *)pBase + 280);
				float y = *((float *)pBase + 272);
				float dx = *((float *)pBase + 274);
				float dy = *((float *)pBase + 275);
				bullets.push_back(entity{
					vec2(x + th_param.GAME_WIDTH / 2,y),
					vec2(dx,dy),
					vec2(w,h)
					});
			}
		}
		pBase += 2320;
		--i;
	} while (i);

}

void th11_player::doEnemyPoll()
{

}

void th11_player::doPowerupPoll()
{

}

void th11_player::doLaserPoll()
{

}

entity th11_player::getPlayerEntity()
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
		entity e = {
			vec2(
				*(float*)(plyrAddr + 0x87C) + th_param.GAME_WIDTH / 2,
				*(float*)(plyrAddr + 0x87C + 4)),
			vec2(),
			vec2(w, h),
			0
		};
		return e;
	}
	return {};
}


