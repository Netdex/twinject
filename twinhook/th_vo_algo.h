#pragma once
#include "th_vector_algo.h"


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
	bool is_calibrated = false;
	/**
	 * \brief Number of frames spent calibrating so far
	 */
	int cal_frames = 0;
	/**
	 * \brief Starting x position when beginning calibration
	 */
	float cal_start_x = -1;
	float player_vel = 0;

	void calibration_init();
	/**
	 * \brief Do one tick of calibration
	 * \return Whether calibration is complete or not
	 */
	bool calibration_tick();
public:
	th_vo_algo(th_player *player) : th_algorithm(player) {}
	~th_vo_algo() = default;

	void on_begin() override;
	void on_tick() override;
	void visualize(IDirect3DDevice9 *d3dDev) override;
};
