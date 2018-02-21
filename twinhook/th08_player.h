#pragma once
#include "th_player.h"
#include "vec2.h"

struct th08_entity
{
	vec2 p;
	vec2 v;
	vec2 sz;			// bullet size
	DWORD me;			// metadata
};

class th08_player : public th_player
{
public:
	std::vector<th08_entity> bullets;
	std::vector<th08_entity> powerups;

	th08_player() {}
	~th08_player() = default;

	void on_init() override;
	void on_tick() override;
	void on_begin_tick() override;
	void on_after_tick() override;
	void draw(IDirect3DDevice9* d3dDev) override;

	void handle_input(BYTE diKeys[256]) override;
	void on_enable_changed(bool enable) override;

private:
	bool render_detailed = false;

	/**
	* \brief Calculate the net weighted vector of forces
	* \param c Player center
	* \param bs Location of main boss
	* \param guide Reference to return guiding vector
	* \param threat Reference to return threat avoidance bector
	*/
	void net_vector(vec2 c, vec2 bs, vec2 &guide, vec2 &threat);
	static float proj_transform(float x);

	static vec2 get_plyr_loc();
	static vec2 get_boss_loc();
};
