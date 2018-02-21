#pragma once
#include "th_player.h"
#include "vec2.h"

struct th07_entity
{
	vec2 p;
	vec2 v;
	vec2 sz;			// bullet size
	DWORD me;			// metadata
};

class th07_player : public th_player
{
public:
	std::vector<th07_entity> bullets;
	std::vector<th07_entity> powerups;

	th07_player() {}
	~th07_player() {}

	void on_init() override;
	void on_tick() override;
	void on_begin_tick() override;
	void on_after_tick() override;
	void draw(IDirect3DDevice9* d3dDev) override;

	void handle_input(BYTE diKeys[256]) override;
	void on_enable_changed(bool enable) override;

private:
	bool render_detailed = false;

	static float th07_player::proj_transform(float x);
	void th07_player::net_vector(vec2 c, vec2 bs, vec2& guide, vec2& threat);

	static vec2 get_plyr_loc();
};
