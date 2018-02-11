#pragma once
#include "d3d_controller.h"

class th08_d3d_controller : d3d_controller
{
protected:
	~th08_d3d_controller() = default;
public:
	void begin(IDirect3DDevice9* d3dDev) override;
	void end(IDirect3DDevice9* d3dDev) override;
	void init(IDirect3DDevice9* d3dDev) override;
};
