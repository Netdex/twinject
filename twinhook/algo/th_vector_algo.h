#pragma once
#include "th_algorithm.h"
#include "../util/vec2.h"

// minimum force required to cause action
const float BOT_ACTION_THRESHOLD = 0.000005f;		// default: 0.000005
													// radius of player
const float BOT_MIN_RADIUS = 4;						// default: 4
													// extra safety space for projection dodging
const float BOT_MACROSAFETY_DELTA = 4;				// default: 2
													// extra safety space for force dodging
const float BOT_MICROSAFETY_DELTA = 5;				// default: 4

													// projection distance multiplier for projection dodging
const float BOT_BULLET_PROJECTION_FACTOR = 20.f;		// default: 20

														// bullet force dodging priority
const float BOT_BULLET_PRIORITY = 20.f;				// default: 10				
													// powerup force guide collection priority
const float BOT_POWERUP_PRIORITY = 0.1f;			// default: 0.1
													// boss follow guide priority
const float BOT_BOSS_TARGET_PRIORITY = 500000000.f;	// default: 100000000

													// maximum y to collect powerups from
const float BOT_POWERUP_MAXY = 300;
// minimum y to collect powerups from
const float BOT_POWERUP_MINY = 70;

// force threshold required to begin focusing
const float FOCUS_FORCE_THRESHOLD = 0.5f;			// default: 0.05

/**
 * \brief Player algorithm using reactive vector dodging
 */
class th_vector_algo : public th_algorithm
{
	

	void net_vector(vec2 c, vec2 bs, vec2& guide, vec2& threat) const;
	static float proj_transform(float x);

public:
	th_vector_algo(th_player *player) : th_algorithm(player) {}
	~th_vector_algo() = default;

	void on_tick() override;
	void visualize(IDirect3DDevice9 *d3dDev) override;
};
