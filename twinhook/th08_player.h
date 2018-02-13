#pragma once
#include "th_player.h"
#include "vec2.h"

struct entity
{
	vec2 p;
	vec2 v;
	vec2 sz;			// bullet size
	DWORD me;			// metadata
};

class th08_player : public th_player
{
public:
	std::vector<entity> bullets;
	std::vector<entity> powerups;

	th08_player();
	~th08_player() = default;


	void on_init() override;
	void on_tick() override;
	void on_before_tick() override;
	void draw(IDirect3DDevice9* d3dDev) override;
};
