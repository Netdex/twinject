#include "../stdafx.h"
#include "th15_player.h"
#include "../directx/IDI8ADevice_Wrapper.h"
#include "../config/th_config.h"
#include "../hook/th_di8_hook.h"
#include "../util/cdraw.h"
#include "../gfx/di8_input_overlay.h"
#include "../algo/th_algorithm.h"
#include "../patch/th15_patch_autobomb.h"
#include "../hook/th_d3d9_hook.h"
#include "gfx/th_info_overlay.h"
#include "patch/th_patch_registry.h"

namespace
{
	/*
	 * TH15 v1.00b layouts:
	 * https://github.com/exphp-share/th-re-data/blob/master/data/th15.v1.00b/statics.json
	 * https://github.com/exphp-share/th-re-data/blob/master/data/th15.v1.00b/type-structs-own.json
	 */
	constexpr uintptr_t TH15_ENEMY_MANAGER_PTR = 0x004E9A80;
	constexpr uintptr_t TH15_ENEMY_LIST_HEAD = 0x180;
	constexpr uintptr_t TH15_ENEMY_NODE_ENTRY = 0x0;
	constexpr uintptr_t TH15_ENEMY_NODE_NEXT = 0x4;
	constexpr uintptr_t TH15_ENEMY_SIZE = 0x574C;
	constexpr uintptr_t TH15_ENEMY_DATA = 0x120C;
	constexpr uintptr_t TH15_ENEMY_POS = 0x44;
	constexpr uintptr_t TH15_ENEMY_VEL = 0x78;
	constexpr uintptr_t TH15_ENEMY_HURTBOX = 0x110;
	constexpr uintptr_t TH15_ENEMY_HITBOX = 0x118;
	constexpr uintptr_t TH15_ENEMY_LIFE = 0x3F74;
	constexpr int TH15_ENEMY_SANITY_LIMIT = 2048;
	constexpr uintptr_t TH15_ITEM_MANAGER_PTR = 0x004E9A9C;
	constexpr uintptr_t TH15_ITEM_ARRAY = 0x10;
	constexpr uintptr_t TH15_ITEM_SIZE = 0xC88;
	// Cross-checked against v1.00b item update/spawn at 0x43F830/0x4408D0.
	constexpr uintptr_t TH15_ITEM_POS = 0xC20;
	constexpr uintptr_t TH15_ITEM_VEL = 0xC2C;
	constexpr uintptr_t TH15_ITEM_STATE = 0xC64;
	constexpr uintptr_t TH15_ITEM_TYPE = 0xC68;
	constexpr size_t TH15_ITEM_COUNT = 0x258;

	static_assert(
		TH15_ITEM_ARRAY + TH15_ITEM_COUNT * TH15_ITEM_SIZE == 0x1D5ED0,
		"TH15 normal item array must end at the cancel item array");

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

void th15_player::onInit()
{
	th_player::onInit();
}

void th15_player::onTick()
{
	th_player::onTick();
}

void th15_player::onBeginTick()
{
	th_player::onBeginTick();
	this->doEnemyPoll();
	this->doPowerupPoll();
	//tle.expand(lasers, bullets);
}

void th15_player::onAfterTick()
{
	th_player::onAfterTick();
}

void th15_player::draw(IDirect3DDevice9* d3dDev)
{
	th_player::draw(d3dDev);
}

void th15_player::handleInput(const BYTE diKeys[256], const BYTE press[256])
{
	th_player::handleInput(diKeys, press);
}

void th15_player::onEnableChanged(bool enable)
{
	th_player::onEnableChanged(enable);

	// this is cheating
	if (enable)
	{
		th_registry::patch("th15_autobomb");
	}
	else
	{
		th_registry::unpatch("th15_autobomb");
	}
}

void th15_player::doEnemyPoll()
{
	uintptr_t manager;
	if (!readValue(TH15_ENEMY_MANAGER_PTR, manager) || !manager)
		return;

	uintptr_t node;
	if (!readValue(manager + TH15_ENEMY_LIST_HEAD, node))
		return;

	std::vector<uintptr_t> visited;
	visited.reserve(64);

	for (int count = 0;
		node && count < TH15_ENEMY_SANITY_LIMIT;
		++count)
	{
		if (!canRead(node, TH15_ENEMY_NODE_NEXT + sizeof(uintptr_t)) ||
			std::find(visited.begin(), visited.end(), node) != visited.end())
			break;
		visited.push_back(node);

		const uintptr_t enemyAddr =
			*(uintptr_t*)(node + TH15_ENEMY_NODE_ENTRY);
		const uintptr_t next =
			*(uintptr_t*)(node + TH15_ENEMY_NODE_NEXT);
		if (canRead(enemyAddr, TH15_ENEMY_SIZE))
		{
			const uintptr_t data = enemyAddr + TH15_ENEMY_DATA;
			const int32_t life = *(int32_t*)(data + TH15_ENEMY_LIFE);
			if (life > 0)
			{
				vec2 center;
				vec2 velocity;
				vec2 size;
				bool haveSize =
					readVec2Unchecked(data + TH15_ENEMY_HITBOX, size) &&
					isReasonableSize(size);
				if (!haveSize)
				{
					haveSize =
						readVec2Unchecked(data + TH15_ENEMY_HURTBOX, size) &&
						isReasonableSize(size);
				}

				const bool haveCenter =
					readVec2Unchecked(data + TH15_ENEMY_POS, center);
				if (haveCenter)
					center.x += th_param.GAME_WIDTH / 2;

				if (haveCenter &&
					readVec2Unchecked(data + TH15_ENEMY_VEL, velocity) &&
					haveSize &&
					isNearPlayfield(center))
				{
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

void th15_player::doPowerupPoll()
{
	uintptr_t manager;
	if (!readValue(TH15_ITEM_MANAGER_PTR, manager) || !manager)
		return;

	const uintptr_t array = manager + TH15_ITEM_ARRAY;
	if (!canRead(array, TH15_ITEM_COUNT * TH15_ITEM_SIZE))
		return;

	for (size_t i = 0; i < TH15_ITEM_COUNT; ++i)
	{
		const uintptr_t item = array + i * TH15_ITEM_SIZE;
		const uint32_t state = *(uint32_t*)(item + TH15_ITEM_STATE);
		const int32_t type = *(int32_t*)(item + TH15_ITEM_TYPE);
		if (!isActiveItemState(state) || type <= 0 || type > 0xF)
			continue;

		vec2 center;
		vec2 velocity;
		if (!readVec2Unchecked(item + TH15_ITEM_POS, center) ||
			!readVec2Unchecked(item + TH15_ITEM_VEL, velocity))
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

player th15_player::getPlayerEntity()
{
	PBYTE *PlayerPtrAddr = (PBYTE*)gs_ptr.plyr_pos; // this should be illegal
	if (*PlayerPtrAddr) {
		PBYTE plyrAddr = *PlayerPtrAddr;

		// we must check if size is readable, because it doesn't initialize immediately
		vec2 size(2.5f, 2.5f);
		if (*(DWORD*)(plyrAddr + 0x2C008))
		{
			size = vec2(
				*(float*)(*(DWORD*)(plyrAddr + 0x2C008) + 4),
				*(float*)(*(DWORD*)(plyrAddr + 0x2C008) + 4));
		}

		circle e{
			vec2(
				*(float*)(plyrAddr + 0x618) + th_param.GAME_WIDTH / 2,
				*(float*)(plyrAddr + 0x61C)),
			vec2(),
			size.x,
		};
		return e;
	}
	return player{ aabb() };
}



