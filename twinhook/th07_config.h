#pragma once

namespace th07_param {
	const int WINDOW_WIDTH = 640;
	const int WINDOW_HEIGHT = 480;
	const int GAME_WIDTH = 384;
	const int GAME_HEIGHT = 448;
	const int GAME_X_OFFSET = 32;
	const int GAME_Y_OFFSET = 16;

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
}