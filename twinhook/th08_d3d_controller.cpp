#include "stdafx.h"
#include "th08_d3d_controller.h"
#include "cdraw.h"
#include "TH08Control.h"
#include "BotControl.h"
#include "BotOverlayRenderer.h"
#include "IDI8A_Wrapper.h"
#include "IDI8ADevice_Wrapper.h"

extern DirectInput8Wrapper *DirectInput8;

void th08_d3d_controller::init(IDirect3DDevice9* d3dDev)
{
	CDraw_Init(d3dDev);
}

void th08_d3d_controller::begin(IDirect3DDevice9* d3dDev)
{
	TH08Control_FrameInit();
	
}

void th08_d3d_controller::end(IDirect3DDevice9* d3dDev)
{
	BYTE diKeys[256];

	Render_BotOverlay(d3dDev);
	if (DirectInput8) {
		if (DirectInput8->DirectInputDevice8->GetDeviceState(256, diKeys) == DI_OK)
		{
			Bot_ProcessControl(diKeys);
		}
		Bot_Tick();
	}
	TH08Control_FrameCleanup();
}


