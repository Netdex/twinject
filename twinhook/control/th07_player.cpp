#include "stdafx.h"
#include "th07_player.h"
#include "util/cdraw.h"
#include "gfx/di8_input_overlay.h"
#include "hook/th_di8_hook.h"
#include "directx/IDI8ADevice_Wrapper.h"
#include "config/th_config.h"
#include "hook/th_d3d9_hook.h"
#include "gfx/th_info_overlay.h"

namespace
{
	/*
	 * TH07 v1.00b layouts:
	 * https://github.com/some100/th07/blob/main/src/th07/EnemyManager.hpp
	 * https://github.com/some100/th07/blob/main/src/th07/ItemManager.hpp
	 * https://github.com/some100/th07/blob/main/src/th07/BulletManager.hpp
	 */
	constexpr uintptr_t TH07_ENEMY_MANAGER = 0x009A9B00;
	constexpr uintptr_t TH07_ENEMIES_OFFSET = 0x4F50;
	constexpr uintptr_t TH07_ENEMY_SIZE = 0x4F48;
	constexpr int TH07_ENEMY_COUNT = 481;
	constexpr uintptr_t TH07_ENEMY_POS = 0x2B0C;
	constexpr uintptr_t TH07_ENEMY_VEL = 0x2B18;
	constexpr uintptr_t TH07_ENEMY_HITBOX = 0x2B3C;
	constexpr uintptr_t TH07_ENEMY_LIFE = 0x2BB8;
	constexpr uintptr_t TH07_ENEMY_FLAGS1 = 0x2E28;
	constexpr uintptr_t TH07_ENEMY_FLAGS2 = 0x2E29;
	constexpr uint8_t TH07_ENEMY_ACTIVE = 0x80;
	constexpr uint8_t TH07_ENEMY_CONTACT_HITBOX = 0x02;
	constexpr uint8_t TH07_ENEMY_NO_COLLISION = 0x08;

	constexpr uintptr_t TH07_ITEM_MANAGER = 0x00575C70;
	constexpr uintptr_t TH07_ITEM_SIZE = 0x288;
	constexpr int TH07_ITEM_COUNT = 1100;
	constexpr uintptr_t TH07_ITEM_POS = 0x24C;
	constexpr uintptr_t TH07_ITEM_TYPE = 0x27C;
	constexpr uintptr_t TH07_ITEM_IN_USE = 0x27D;
	constexpr int8_t TH07_ITEM_POINT_BULLET = 6;

	constexpr uintptr_t TH07_BULLET_MANAGER = 0x0062F958;
	constexpr uintptr_t TH07_LASERS_OFFSET = 0x366628;
	constexpr uintptr_t TH07_LASER_SIZE = 0x4EC;
	constexpr int TH07_LASER_COUNT = 64;
	constexpr uintptr_t TH07_LASER_POS = 0x498;
	constexpr uintptr_t TH07_LASER_ANGLE = 0x4A4;
	constexpr uintptr_t TH07_LASER_START_OFFSET = 0x4A8;
	constexpr uintptr_t TH07_LASER_END_OFFSET = 0x4AC;
	constexpr uintptr_t TH07_LASER_WIDTH = 0x4B4;
	constexpr uintptr_t TH07_LASER_HITBOX_START_TIME = 0x4C4;
	constexpr uintptr_t TH07_LASER_HITBOX_END_TIME = 0x4D0;
	constexpr uintptr_t TH07_LASER_IN_USE = 0x4D4;
	constexpr uintptr_t TH07_LASER_TIMER = 0x4E0;
	constexpr uintptr_t TH07_LASER_STATE = 0x4E8;
	constexpr uint8_t TH07_LASER_SPAWNING = 0;
	constexpr uint8_t TH07_LASER_ACTIVE = 1;
	constexpr uint8_t TH07_LASER_DESPAWNING = 2;

	static_assert(
		TH07_ENEMIES_OFFSET + TH07_ENEMY_COUNT * TH07_ENEMY_SIZE == 0x954598,
		"TH07 enemy layout no longer reaches the documented boss pointer offset");
	static_assert(
		TH07_LASERS_OFFSET + TH07_LASER_COUNT * TH07_LASER_SIZE == 0x37A128,
		"TH07 laser layout no longer reaches the documented manager tail");

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

void th07_player::onInit()
{
	th_player::onInit();
}

void th07_player::onTick()
{
	th_player::onTick();
}

void th07_player::onBeginTick()
{
	th_player::onBeginTick();
	this->doEnemyPoll();
	this->doPowerupPoll();
	this->doLaserPoll();
}

void th07_player::onAfterTick()
{
	th_player::onAfterTick();
}

void th07_player::handleInput(const BYTE diKeys[256], const BYTE press[256])
{
	th_player::handleInput(diKeys, press);
}

void th07_player::onEnableChanged(bool enable)
{
	th_player::onEnableChanged(enable);
}

player th07_player::getPlayerEntity()
{
	PBYTE PlayerPtrAddr = (PBYTE)this->gs_ptr.plyr_pos;

	vec2 sz(5, 5);
	aabb a{
		vec2(*(float*)PlayerPtrAddr - th_param.GAME_X_OFFSET,
			 *(float*)(PlayerPtrAddr + 4) - th_param.GAME_Y_OFFSET) - sz / 2,
		vec2(),
		sz,
	};
	return player{ a };
}

void th07_player::doEnemyPoll()
{
	const uintptr_t firstEnemy = TH07_ENEMY_MANAGER + TH07_ENEMIES_OFFSET;
	if (!canRead(firstEnemy, TH07_ENEMY_COUNT * TH07_ENEMY_SIZE))
		return;

	for (int i = 0; i < TH07_ENEMY_COUNT; ++i)
	{
		const uintptr_t enemyAddr = firstEnemy + i * TH07_ENEMY_SIZE;
		const uint8_t flags1 = *(uint8_t*)(enemyAddr + TH07_ENEMY_FLAGS1);
		const uint8_t flags2 = *(uint8_t*)(enemyAddr + TH07_ENEMY_FLAGS2);
		const int32_t life = *(int32_t*)(enemyAddr + TH07_ENEMY_LIFE);

		if (!(flags1 & TH07_ENEMY_ACTIVE) ||
			!(flags2 & TH07_ENEMY_CONTACT_HITBOX) ||
			(flags2 & TH07_ENEMY_NO_COLLISION) ||
			life <= 0)
			continue;

		vec2 center;
		vec2 velocity;
		vec2 size;
		if (!readVec2Unchecked(enemyAddr + TH07_ENEMY_POS, center) ||
			!readVec2Unchecked(enemyAddr + TH07_ENEMY_VEL, velocity) ||
			!readVec2Unchecked(enemyAddr + TH07_ENEMY_HITBOX, size) ||
			!isNearPlayfield(center) ||
			size.x <= 0 || size.y <= 0 || size.x > 256 || size.y > 256)
			continue;

		aabb bounds{
			center - size / 2,
			velocity,
			size,
		};
		enemies.push_back(enemy{ bounds });
	}
}

void th07_player::doPowerupPoll()
{
	if (!canRead(TH07_ITEM_MANAGER, TH07_ITEM_COUNT * TH07_ITEM_SIZE))
		return;

	for (int i = 0; i < TH07_ITEM_COUNT; ++i)
	{
		const uintptr_t itemAddr = TH07_ITEM_MANAGER + i * TH07_ITEM_SIZE;
		if (!*(uint8_t*)(itemAddr + TH07_ITEM_IN_USE))
			continue;

		const int8_t itemType = *(int8_t*)(itemAddr + TH07_ITEM_TYPE);
		if (itemType < 0)
			continue;

		vec2 center;
		if (!readVec2Unchecked(itemAddr + TH07_ITEM_POS, center) ||
			!isNearPlayfield(center))
			continue;

		const vec2 size(6, 6);
		aabb bounds{
			center - size / 2,
			vec2(),
			size,
		};
		powerups.push_back(powerup{
			bounds,
			itemType == TH07_ITEM_POINT_BULLET ? 1 : 0,
		});
	}
}

void th07_player::doLaserPoll()
{
	const uintptr_t firstLaser = TH07_BULLET_MANAGER + TH07_LASERS_OFFSET;
	if (!canRead(firstLaser, TH07_LASER_COUNT * TH07_LASER_SIZE))
		return;

	for (int i = 0; i < TH07_LASER_COUNT; ++i)
	{
		const uintptr_t laserAddr = firstLaser + i * TH07_LASER_SIZE;
		if (!*(int32_t*)(laserAddr + TH07_LASER_IN_USE))
			continue;

		const uint8_t state = *(uint8_t*)(laserAddr + TH07_LASER_STATE);
		const int32_t timer = *(int32_t*)(laserAddr + TH07_LASER_TIMER);
		const int32_t hitboxStart =
			*(int32_t*)(laserAddr + TH07_LASER_HITBOX_START_TIME);
		const int32_t hitboxEnd =
			*(int32_t*)(laserAddr + TH07_LASER_HITBOX_END_TIME);
		const bool hasHitbox =
			(state == TH07_LASER_SPAWNING && timer >= hitboxStart) ||
			state == TH07_LASER_ACTIVE ||
			(state == TH07_LASER_DESPAWNING && timer < hitboxEnd);
		if (!hasHitbox)
			continue;

		vec2 origin;
		float angle = *(float*)(laserAddr + TH07_LASER_ANGLE);
		float startOffset = *(float*)(laserAddr + TH07_LASER_START_OFFSET);
		float endOffset = *(float*)(laserAddr + TH07_LASER_END_OFFSET);
		float width = *(float*)(laserAddr + TH07_LASER_WIDTH);
		if (!readVec2Unchecked(laserAddr + TH07_LASER_POS, origin) ||
			!std::isfinite(angle) ||
			!std::isfinite(startOffset) ||
			!std::isfinite(endOffset) ||
			!std::isfinite(width))
			continue;

		const float length = endOffset - startOffset;
		if (startOffset < 0 || length <= 0 || length > 1024 ||
			width <= 0 || width > 256)
			continue;

		const vec2 direction(cos(angle), sin(angle));
		obb bounds{
			origin + direction * startOffset,
			length,
			width / 2,
			angle,
			vec2(),
		};
		lasers.push_back(laser{ bounds });
	}
}

void th07_player::draw(IDirect3DDevice9* d3dDev)
{
	th_player::draw(d3dDev);
}

