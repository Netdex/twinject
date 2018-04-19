#pragma once
#include <vector>

const int SAMPLE_SIZE = 64;

union th_kbd_state
{
	struct {
		bool shot;		// 0
		bool bomb;		// 1
		bool slow;		// 2
		bool skip;		// 3
		bool up;		// 4
		bool left;		// 5
		bool down;		// 6
		bool right;		// 7
	};
	bool keys[8];
};

struct vec2
{
	float x, y;
};

struct sample
{
	int sampleCount;
	std::vector<vec2> pos;
	std::vector<vec2> vel;
	th_kbd_state keys;
};

std::vector<sample> load_samples(std::string filename);