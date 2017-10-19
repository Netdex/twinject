#include "stdafx.h"
#include "D3D9Control.h"
#include "cdraw.h"
#include "IDI8A_Wrapper.h"
#include "IDI8ADevice_Wrapper.h"
#include "BotControl.h"
#include "TH08Control.h"
#include "BotOverlayRenderer.h"

extern DirectInput8Wrapper *DirectInput8;
extern std::vector<entity> TH08_Bullets;

void D3D9_Init_Hook(IDirect3DDevice9 *d3dDev)
{
	CDraw_Init(d3dDev);
}

void D3D9_BeginScene_Hook(IDirect3DDevice9 *d3dDev)
{
	TH08Control_FrameInit();
}

void D3D9_EndScene_Hook(IDirect3DDevice9 *d3dDev)
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