#include "stdafx.h"
#include "th11_player.h"
#include "config/th_config.h"
#include "hook/th_di8_hook.h"

namespace
{
	/*
	 * TH11 v1.00a layouts:
	 * https://github.com/exphp-share/th-re-data/blob/master/data/th11.v1.00a/statics.json
	 * https://github.com/exphp-share/th-re-data/blob/master/data/th11.v1.00a/type-structs-own.json
	 */
	constexpr uintptr_t TH11_ENEMY_MANAGER_PTR = 0x004A8D7C;
	constexpr uintptr_t TH11_ENEMY_LIST_HEAD = 0x68;
	constexpr uintptr_t TH11_ENEMY_NODE_ENTRY = 0x0;
	constexpr uintptr_t TH11_ENEMY_NODE_NEXT = 0x4;
	constexpr uintptr_t TH11_ENEMY_DATA_SIZE = 0x163C;
	constexpr uintptr_t TH11_ENEMY_POS = 0x34;
	constexpr uintptr_t TH11_ENEMY_SPEED = 0x4C;
	constexpr uintptr_t TH11_ENEMY_ANGLE = 0x50;
	constexpr uintptr_t TH11_ENEMY_HURTBOX = 0xD0;
	constexpr uintptr_t TH11_ENEMY_HITBOX = 0xD8;
	constexpr uintptr_t TH11_ENEMY_LIFE = 0x14F0;
	constexpr int TH11_ENEMY_SANITY_LIMIT = 2048;
	constexpr uintptr_t TH11_ITEM_MANAGER_PTR = 0x004A8E90;
	constexpr uintptr_t TH11_ITEM_ARRAY = 0x14;
	constexpr uintptr_t TH11_ITEM_SIZE = 0x478;
	// Cross-checked against v1.00a item update/spawn at 0x423550/0x424230.
	constexpr uintptr_t TH11_ITEM_POS = 0x434;
	constexpr uintptr_t TH11_ITEM_VEL = 0x440;
	constexpr uintptr_t TH11_ITEM_STATE = 0x464;
	constexpr uintptr_t TH11_ITEM_TYPE = 0x468;
	constexpr size_t TH11_ITEM_COUNT = 0x96;

	static_assert(
		TH11_ITEM_ARRAY + TH11_ITEM_COUNT * TH11_ITEM_SIZE == 0x29E64,
		"TH11 normal item array must end at the cancel item array");

	bool canRead(uintptr_t addr, size_t size)
	{
		const uintptr_t end = addr + size;
		if (!addr || !size || end < addr)
			return false;

		for (uintptr_t cur = addr; cur < end;)
		{
			MEMORY_BASIC_INFORMATION mbi{};
			if (!VirtualQuery((void*)cur, &mbi, sizeof(mbi)) ||
				mbi.State != MEM_COMMIT ||
				(mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)))
				return false;

			const uintptr_t regionEnd =
				(uintptr_t)mbi.BaseAddress + mbi.RegionSize;
			if (regionEnd <= cur)
				return false;
			cur = regionEnd;
		}
		return true;
	}

	template <typename T>
	bool readValue(uintptr_t addr, T& value)
	{
		if (!canRead(addr, sizeof(T)))
			return false;
		value = *(T*)addr;
		return true;
	}

	bool readVec2Unchecked(uintptr_t addr, vec2& value)
	{
		const float x = *(float*)addr;
		const float y = *(float*)(addr + sizeof(float));
		if (!std::isfinite(x) || !std::isfinite(y))
			return false;
		value = vec2(x, y);
		return true;
	}

	bool isNearPlayfield(const vec2& p)
	{
		return p.x >= -128 && p.x <= th_param.GAME_WIDTH + 128 &&
			p.y >= -128 && p.y <= th_param.GAME_HEIGHT + 128;
	}

	bool isReasonableSize(const vec2& size)
	{
		return size.x > 0 && size.y > 0 && size.x <= 256 && size.y <= 256;
	}

	bool isActiveItemState(uint32_t state)
	{
		return state == 1 || state == 3 || state == 4;
	}
}


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
				vec2 sz(w, h);
				aabb a{
					vec2(x + th_param.GAME_WIDTH / 2,y) - sz / 2,
					vec2(dx,dy),
					sz
				};
				bullet b{ a };
				bullets.push_back(b);
			}
		}
		pBase += 2320;
		--i;
	} while (i);

}

void th11_player::doEnemyPoll()
{
	uintptr_t manager;
	if (!readValue(TH11_ENEMY_MANAGER_PTR, manager) || !manager)
		return;

	uintptr_t node;
	if (!readValue(manager + TH11_ENEMY_LIST_HEAD, node))
		return;

	std::vector<uintptr_t> visited;
	visited.reserve(64);

	for (int count = 0;
		node && count < TH11_ENEMY_SANITY_LIMIT;
		++count)
	{
		if (!canRead(node, TH11_ENEMY_NODE_NEXT + sizeof(uintptr_t)) ||
			std::find(visited.begin(), visited.end(), node) != visited.end())
			break;
		visited.push_back(node);

		const uintptr_t data =
			*(uintptr_t*)(node + TH11_ENEMY_NODE_ENTRY);
		const uintptr_t next =
			*(uintptr_t*)(node + TH11_ENEMY_NODE_NEXT);
		if (canRead(data, TH11_ENEMY_DATA_SIZE))
		{
			const int32_t life = *(int32_t*)(data + TH11_ENEMY_LIFE);
			if (life > 0)
			{
				vec2 center;
				vec2 size;
				bool haveSize =
					readVec2Unchecked(data + TH11_ENEMY_HITBOX, size) &&
					isReasonableSize(size);
				if (!haveSize)
				{
					haveSize =
						readVec2Unchecked(data + TH11_ENEMY_HURTBOX, size) &&
						isReasonableSize(size);
				}

				const float speed = *(float*)(data + TH11_ENEMY_SPEED);
				const float angle = *(float*)(data + TH11_ENEMY_ANGLE);
				const bool haveCenter =
					readVec2Unchecked(data + TH11_ENEMY_POS, center);
				if (haveCenter)
					center.x += th_param.GAME_WIDTH / 2;

				if (haveCenter &&
					haveSize &&
					isNearPlayfield(center) &&
					std::isfinite(speed) &&
					std::isfinite(angle))
				{
					const vec2 velocity(
						speed * cos(angle),
						speed * sin(angle));
					aabb bounds{
						center - size / 2,
						velocity,
						size,
					};
					enemies.push_back(enemy{ bounds });
				}
			}
		}
		node = next;
	}
}

void th11_player::doPowerupPoll()
{
	uintptr_t manager;
	if (!readValue(TH11_ITEM_MANAGER_PTR, manager) || !manager)
		return;

	const uintptr_t array = manager + TH11_ITEM_ARRAY;
	if (!canRead(array, TH11_ITEM_COUNT * TH11_ITEM_SIZE))
		return;

	for (size_t i = 0; i < TH11_ITEM_COUNT; ++i)
	{
		const uintptr_t item = array + i * TH11_ITEM_SIZE;
		const uint32_t state = *(uint32_t*)(item + TH11_ITEM_STATE);
		const int32_t type = *(int32_t*)(item + TH11_ITEM_TYPE);
		if (!isActiveItemState(state) || type <= 0 || type > 0xB)
			continue;

		vec2 center;
		vec2 velocity;
		if (!readVec2Unchecked(item + TH11_ITEM_POS, center) ||
			!readVec2Unchecked(item + TH11_ITEM_VEL, velocity))
			continue;

		center.x += th_param.GAME_WIDTH / 2;
		if (!isNearPlayfield(center))
			continue;

		const vec2 size(6.0f, 6.0f);
		aabb bounds{
			center - size / 2,
			velocity,
			size,
		};
		powerups.push_back(powerup{ bounds });
	}

}

void th11_player::doLaserPoll()
{

}

player th11_player::getPlayerEntity()
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


