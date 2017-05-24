#include "stdafx.h"
#include "Direct3DCreate9Hook.h"
#include "detour.h"
#include "ID3D9_Wrapper.h"
#include "cdraw.h"
#include "BulletProcessHook.h"
#include "IDI8A_Wrapper.h"

extern std::vector<bullet> bullets;

Direct3D9Wrapper *Direct3D9;

Direct3DCreate9_t Direct3DCreate9_Original = Direct3DCreate9;
PBYTE pDirect3DCreate9;

extern DirectInput8Wrapper *DirectInput8;

void Init_Hook(IDirect3DDevice9 *d3dDev)
{
	CDraw_InitSolidTexture(d3dDev);
	CDraw_InitFont(d3dDev, 17, L"Consolas");
	CDraw_InitLine(d3dDev);
}
void BeginScene_Hook(IDirect3DDevice9 *d3dDev)
{

}
void EndScene_Hook(IDirect3DDevice9 *d3dDev)
{
	char buf[256];
	CDraw_Rect(d3dDev, 445, 250, 640, 480, D3DCOLOR_ARGB(200, 0, 0, 0));
	CDraw_Text("TWINJECT [ NETDEX ]", D3DCOLOR_ARGB(255, 0, 255, 255), 450, 255, 640, 480);

	BYTE  diKeys[256];

	if (DirectInput8) {
		if (DirectInput8->DirectInputDevice8->GetDeviceState(256, diKeys) == DI_OK)
		{
			int count = 0;
			for (int i = 0; i < 256; i++)
			{
				if (diKeys[i]) count++;
			}
			sprintf_s(buf, 256, "KEY_COUNT: %d", count);
			CDraw_Text(buf, D3DCOLOR_ARGB(255, 255, 255, 255), 450, 270, 640, 480);
		}
	}
	sprintf_s(buf, 256, "BULLET_COUNT: %d", bullets.size());
	CDraw_Text(buf, D3DCOLOR_ARGB(255, 255, 255, 255), 450, 285, 640, 480);

	for (auto i = bullets.begin(); i != bullets.end(); ++i)
	{
		CDraw_Rect(d3dDev, (*i).x - 2 + GAME_X_OFFSET, (*i).y - 2 + GAME_Y_OFFSET, 4, 4, D3DCOLOR_ARGB(255, 255, 2, 200));
		CDraw_Line((*i).x + GAME_X_OFFSET, (*i).y + GAME_Y_OFFSET,
			(*i).x + (*i).vx * 5 + GAME_X_OFFSET, (*i).y + (*i).vy * 5 + GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));
	}
	bullets.clear();
}

IDirect3D9* WINAPI Direct3DCreate9_Hook(UINT sdkVers)
{
	logfs("D3D9: Feeding fake IDirect3D9");
	IDirect3D9 *legit = Direct3DCreate9_Original(sdkVers);
	Direct3D9 = new Direct3D9Wrapper(legit, Init_Hook, BeginScene_Hook, EndScene_Hook);
	return Direct3D9;
}
void Hook_D3D9_Direct3DCreate9()
{
	if (DetourFunction(&(PVOID&)Direct3DCreate9_Original, Direct3DCreate9_Hook))
		logfs("Detours: Hooked Direct3DCreate9");
	else
		logfs("Detours: Failed to hook Direct3DCreate9");
}