#pragma once

#include <d3d9.h>

class d3d_controller
{
protected:
	~d3d_controller() = default;
public:
	d3d_controller(){}

	virtual void begin(IDirect3DDevice9 *d3dDev) = 0;
	virtual void end(IDirect3DDevice9 *d3dDev) = 0;
	virtual void init(IDirect3DDevice9 *d3dDev) = 0;
};