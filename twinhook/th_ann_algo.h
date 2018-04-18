#pragma once
#include "th_algorithm.h"
#include "vec2.h"
#include "th_player.h"

#pragma warning(push, 0)        
#include "tiny_dnn/tiny_dnn.h"
#pragma warning(pop)

const int SAMPLE_SIZE = 64;
const std::string SAMPLE_FILE = "nn_response.samp";

class th_ann_algo : public th_algorithm
{
	bool hit_circle = false;
public:
	struct sample
	{
		// relative locations to players of critical points
		vec2 critpt[SAMPLE_SIZE];
		// velocities of critical points
		vec2 ptvel[SAMPLE_SIZE];
		// key presses
		th_kbd_state keys;
	};

	// Q: Why don't you use proper encapsulation?
	// A: Because I'm the only one who works on this code,
	//    and I (hopefully) won't accidentally change things that
	//    shouldn't be changed
	//    Designing proper APIs is actually very difficult,
	//    and I'd rather put that time into giving twinject
	//    more features
	//    At the bare minimum, twinject's functionality should be 
	//    maintainable and understandable to anybody with a 
	//    working knowledge of C++ and x86 architecture

	std::vector<sample> samples;
	tiny_dnn::network<tiny_dnn::sequential> nnetwork;

	bool is_sampling = false;

	th_ann_algo(th_player *player) : th_algorithm(player) {}
	th_ann_algo(th_player *player, bool useHitCircles) 
		: th_algorithm(player), hit_circle(useHitCircles) {}

	~th_ann_algo() = default;

	void on_begin() override;
	void on_tick() override;
	void visualize(IDirect3DDevice9 *d3dDev) override;
	void handle_input(const BYTE diKeys[256], const BYTE press[256]) override;

	// sampling
	void set_sampling(bool sampling);

	bool save_samples(std::string filename);
	bool load_samples(std::string filename);
	
	// training

	// prediction
};
