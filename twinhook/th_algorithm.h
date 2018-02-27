#pragma once

class th_player;

class th_algorithm
{
protected:
	th_player *player;
public:
	th_algorithm(th_player *player) : player(player) {}
	virtual ~th_algorithm() {}
	
	virtual void on_begin() {};
	virtual void on_tick() = 0;
	virtual void visualize(IDirect3DDevice9* d3dDev){}
};
