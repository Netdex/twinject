#pragma once
#include "th_player.h"
#include "vec2.h"



class th08_player : public th_player
{
public:
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

	/**
	* \brief Calculate the net weighted vector of forces
	* \param c Player center
	* \param bs Location of main boss
	* \param guide Reference to return guiding vector
	* \param threat Reference to return threat avoidance vector
	*/
	void net_vector(vec2 c, vec2 bs, vec2 &guide, vec2 &threat);
	static float proj_transform(float x);

	vec2 get_plyr_loc() override;
	vec2 get_boss_loc();
};
