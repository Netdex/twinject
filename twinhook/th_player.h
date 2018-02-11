#pragma once
#include <unordered_map>
#include "data.h"

class th_player
{
protected:
	~th_player() = default;
private:
	std::unordered_map<std::string, data> state;

public:
	th_player() {}

	virtual void init() {}
	virtual void hook() {}
	virtual void before_tick() {}
	virtual void tick() {}
	virtual void handle_input(BYTE keys[256]) {}
	virtual void draw(IDirect3DDevice9 *d3dDev) {}
};
