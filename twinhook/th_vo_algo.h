#pragma once
#include "th_vector_algo.h"


class th_vo_algo : public th_algorithm
{
	const int CALIBRATION_INTERVAL = 500;

	bool calibrated = false;
	int calibration_ticks = 0;
	
public:
	th_vo_algo(th_player *player) : th_algorithm(player) {}
	~th_vo_algo() = default;

	void on_tick() override;
	void visualize(IDirect3DDevice9 *d3dDev) override;
};
