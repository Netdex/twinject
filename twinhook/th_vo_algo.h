#pragma once
#include "th_vector_algo.h"
#include "th_player.h"

/* Visualization Constants */
static const float VEC_FIELD_RESOLUTION = 32.f;
static const float MAX_FRAMES_TILL_COLLISION = 200.f;	// used for coloring vector field

/* Algorithmic Constants */
static const float SQRT_2 = sqrt(2.f);
/*
* Unit velocity as a result of moving in specified direction
* [0]: Hold, [1]: Up, [2]: Down, [3]: Left, [4]: Right,
* [5]: Top-left, [6]: Top-right, [7]: Bottom-left, [8]: Bottom-right,
* [9]: F Up, [10]: F Down, [11]: F Left, [12]: F Right, [13]: F Top-left,
* [14]: F top-right, [15]: F bottom-left, [16]: F bottom-right
*/
static const vec2 direction_vel[] =
{
	vec2(0,0), vec2(0,-1), vec2(0,1), vec2(-1,0), vec2(1,0),
	vec2(-SQRT_2, -SQRT_2), vec2(SQRT_2, -SQRT_2),
	vec2(-SQRT_2, SQRT_2), vec2(SQRT_2, SQRT_2),
	vec2(0,-1), vec2(0,1), vec2(-1,0), vec2(1,0),
	vec2(-SQRT_2, -SQRT_2), vec2(SQRT_2, -SQRT_2),
	vec2(-SQRT_2, SQRT_2), vec2(SQRT_2, SQRT_2)
};

static const int n_dirs = sizeof direction_vel / sizeof vec2;

static const bool focused_dir[] = {
	false,false,false,false,false,false,false,false,false,
	true,true,true,true,true,true,true,true
};

static const BYTE dir_keys[][3] = {
	{ DIK_LSHIFT,	NULL,			NULL },	// focus by default
	{ DIK_UP,		NULL,			NULL },
	{ DIK_DOWN,		NULL,			NULL },
	{ DIK_LEFT,		NULL,			NULL },
	{ DIK_RIGHT,	NULL,			NULL },
	{ DIK_UP,		DIK_LEFT,		NULL },
	{ DIK_UP,		DIK_RIGHT,		NULL },
	{ DIK_DOWN,		DIK_LEFT,		NULL },
	{ DIK_DOWN,		DIK_RIGHT,		NULL },
	{ DIK_UP,		NULL,			DIK_LSHIFT },
	{ DIK_DOWN,		NULL,			DIK_LSHIFT },
	{ DIK_LEFT,		NULL,			DIK_LSHIFT },
	{ DIK_RIGHT,	NULL,			DIK_LSHIFT },
	{ DIK_UP,		DIK_LEFT,		DIK_LSHIFT },
	{ DIK_UP,		DIK_RIGHT,		DIK_LSHIFT },
	{ DIK_DOWN,		DIK_LEFT,		DIK_LSHIFT },
	{ DIK_DOWN,		DIK_RIGHT,		DIK_LSHIFT }
};

static const BYTE ctrl_keys[] = { DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT, DIK_LSHIFT };

/**
 * \brief Implementation of velocity obstacle based algorithm
 *
 * Overview:
 * The player has 5 possible velocity states excluding combination states,
 * UP, DOWN, LEFT, RIGHT, HOLD
 *
 * First we must calibrate the algorithm by determining the player velocity, by
 * frame division.
 * TODO: We must add an on_algo_activate event in order to allow time for
 *       calibration, or bind an explicit calibrate button.
 *
 * Each velocity state then projected for collisions with obstacles. The state that
 * results in a collision being the furthest away (greedy) is the desired action.
 */
class th_vo_algo : public th_algorithm
{
	/* Adaptibility Parameters */
	// Should we use hitcircles instead of hitboxes
	bool hit_circle = false;

	/* Calibration Parameters */
	bool is_calibrated = false;
	// Number of frames spent calibrating so far
	int cal_frames = 0;
	// Starting x position when beginning calibration
	float cal_start_x = -1;
	float player_vel = 0;
	float player_f_vel = 0;

	void calibration_init();

	/**
	* \brief Do one tick of calibration
	* \return Whether calibration is complete or not
	*/
	bool calibration_tick();

	/* Visualization Parameters*/


public:
	th_vo_algo(th_player *player) : th_algorithm(player) {}
	th_vo_algo(th_player *player, bool hit_circle) : th_algorithm(player), hit_circle(hit_circle) {}

	~th_vo_algo() = default;

	void on_begin() override;
	void on_tick() override;
	void visualize(IDirect3DDevice9 *d3dDev) override;
};
