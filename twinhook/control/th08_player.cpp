#include "../stdafx.h"
#include "th08_player.h"
#include "../directx/IDI8ADevice_Wrapper.h"
#include "../patch/th08_patch_autobomb.h"
#include "../config/th_config.h"
#include "../hook/th_di8_hook.h"
#include "../util/cdraw.h"
#include "../gfx/di8_input_overlay.h"
#include "../algo/th_algorithm.h"
#include "../hook/th_d3d9_hook.h"
#include "gfx/th_info_overlay.h"
#include "patch/th_patch_registry.h"

namespace
{
	/*
	 * TH08 v1.00d layouts:
	 * https://github.com/exphp-share/th-re-data/blob/master/data/th08.v1.00d/statics.json
	 * https://github.com/exphp-share/th-re-data/blob/master/data/th08.v1.00d/type-structs-own.json
	 */
	constexpr uintptr_t TH08_ENEMY_MANAGER = 0x00577F20;
	constexpr uintptr_t TH08_ENEMIES_OFFSET = 0x53D0;
	constexpr uintptr_t TH08_ENEMIES_END_OFFSET = 0x9DCDA0;
	constexpr uintptr_t TH08_ENEMY_LIST_HEADS = 0x9DCEDC;
	constexpr uintptr_t TH08_ENEMY_SIZE = 0x53D0;
	constexpr int TH08_ENEMY_COUNT = 481;
	constexpr int TH08_ENEMY_LAYERS = 4;
	constexpr uintptr_t TH08_ENEMY_NEXT = 0x0;
	constexpr uintptr_t TH08_ENEMY_VEL = 0x2D40;
	constexpr uintptr_t TH08_ENEMY_HITBOX = 0x2D70;
	constexpr uintptr_t TH08_ENEMY_POS = 0x2D88;
	constexpr uintptr_t TH08_ENEMY_LIFE = 0x2DFC;

	static_assert(
		TH08_ENEMIES_OFFSET + TH08_ENEMY_COUNT * TH08_ENEMY_SIZE ==
			TH08_ENEMIES_END_OFFSET,
		"TH08 enemy layout no longer reaches the documented boss pointer offset");

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
}

/*
* TODO
*
* X - BUG: Bullets that come directly will not be dodged (fixed?)
* X - Bullet sizes contribute to action range (fixed?)
*
* - Handle focusing better
* - Lasers
* - Reimplement shrinking radius system
* - Implement simulated annealing tuning of parameters
* - Decrease focus dodge force (ie. focus more often)
* - Implement player velocity determination/calibration
* - Enemy detection and targeting (partially implemented through boss detection)
*/

void th08_player::onInit()
{
	th_player::onInit();
}

void th08_player::onTick()
{
	th_player::onTick();
}

void th08_player::onBeginTick()
{
	th_player::onBeginTick();
	this->doEnemyPoll();
}

void th08_player::onAfterTick()
{
	th_player::onAfterTick();
}

void th08_player::draw(IDirect3DDevice9* d3dDev)
{
	th_player::draw(d3dDev);
}

void th08_player::handleInput(const BYTE diKeys[256], const BYTE press[256])
{
	th_player::handleInput(diKeys, press);
}

void th08_player::onEnableChanged(bool enable)
{
	th_player::onEnableChanged(enable);

	if (enable)
	{
		th_registry::patch("th08_autobomb");
	}
	else
	{
		th_registry::unpatch("th08_autobomb");
	}
}

player th08_player::getPlayerEntity()
{
	PBYTE PlayerPtrAddr = (PBYTE)this->gs_ptr.plyr_pos;

	vec2 sz(5, 5);

	aabb a{
		vec2(*(float*)PlayerPtrAddr - th_param.GAME_X_OFFSET,
			 *(float*)(PlayerPtrAddr + 4) - th_param.GAME_Y_OFFSET) - sz / 2,
		vec2(),
		sz, // hard-coded player size
	};
	return player{ a };
}

void th08_player::doEnemyPoll()
{
	const uintptr_t firstEnemy = TH08_ENEMY_MANAGER + TH08_ENEMIES_OFFSET;
	const uintptr_t enemyEnd = TH08_ENEMY_MANAGER + TH08_ENEMIES_END_OFFSET;
	const uintptr_t listHeads = TH08_ENEMY_MANAGER + TH08_ENEMY_LIST_HEADS;
	if (!canRead(firstEnemy, enemyEnd - firstEnemy) ||
		!canRead(listHeads, TH08_ENEMY_LAYERS * sizeof(uintptr_t)))
		return;

	std::vector<uintptr_t> visited;
	visited.reserve(TH08_ENEMY_COUNT);

	for (int layer = 0; layer < TH08_ENEMY_LAYERS; ++layer)
	{
		uintptr_t enemyAddr =
			*(uintptr_t*)(listHeads + layer * sizeof(uintptr_t));
		for (int count = 0; enemyAddr && count < TH08_ENEMY_COUNT; ++count)
		{
			if (enemyAddr < firstEnemy || enemyAddr >= enemyEnd ||
				(enemyAddr - firstEnemy) % TH08_ENEMY_SIZE != 0 ||
				std::find(visited.begin(), visited.end(), enemyAddr) != visited.end())
				break;
			visited.push_back(enemyAddr);

			const uintptr_t next =
				*(uintptr_t*)(enemyAddr + TH08_ENEMY_NEXT);
			const int32_t life = *(int32_t*)(enemyAddr + TH08_ENEMY_LIFE);
			if (life > 0)
			{
				vec2 center;
				vec2 velocity;
				vec2 size;
				if (readVec2Unchecked(enemyAddr + TH08_ENEMY_POS, center) &&
					readVec2Unchecked(enemyAddr + TH08_ENEMY_VEL, velocity) &&
					readVec2Unchecked(enemyAddr + TH08_ENEMY_HITBOX, size) &&
					isNearPlayfield(center) &&
					size.x > 0 && size.y > 0 && size.x <= 256 && size.y <= 256)
				{
					aabb bounds{
						center - size / 2,
						velocity,
						size,
					};
					enemies.push_back(enemy{ bounds });
				}
			}
			enemyAddr = next;
		}
	}
}

