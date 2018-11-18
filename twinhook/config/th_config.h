#pragma once

// global parameters that should be constant thru every game
extern struct th_param {
	const float EXPECTED_WINDOW_WIDTH = 640;
	const float EXPECTED_WINDOW_HEIGHT = 480;
	float WINDOW_WIDTH = EXPECTED_WINDOW_WIDTH;
	float WINDOW_HEIGHT = EXPECTED_WINDOW_HEIGHT;
	const float GAME_WIDTH = 384;
	const float GAME_HEIGHT = 448;
	const float GAME_X_OFFSET = 32;
	const float GAME_Y_OFFSET = 16;
} th_param;