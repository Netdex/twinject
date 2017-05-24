#include "stdafx.h"
#include "DirectInput8CreateHook.h"
#include "IDI8A_Wrapper.h"
#include "detour.h"

PBYTE pSub_447A37 = (PBYTE)0x00446A37;
sub_446A37_t sub_447A37_Original = (sub_446A37_t)pSub_447A37;

int __fastcall sub_446A37_Hook(int a1)
{
	logfs("DI8Init: Hooking second stage DirectInputCreate");
	Hook_DInput8_DirectInput8Create();
	return sub_447A37_Original(a1);
}
void Hook_TH08_sub_447A37()
{
	if (DetourFunction(&(PVOID&)sub_447A37_Original, sub_446A37_Hook))
		logfs("Detours: Hooked sub_447A37");
	else
		logfs("Detours: Failed to hook sub_447A37");
}

DirectInput8Wrapper *DirectInput8;

DirectInput8Create_t DirectInput8Create_Original = DirectInput8Create;
HRESULT __stdcall DirectInput8Create_Hook(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter)
{
	logfs("DI8Create: Feeding fake DirectInput");
	LPDIRECTINPUT8A legit;
	HRESULT result = DirectInput8Create_Original(hinst, dwVersion, riidltf, (LPVOID*)&legit, punkOuter);
	DirectInput8 = new DirectInput8Wrapper(legit);
	*ppvOut = DirectInput8;
	return result;
}
void Hook_DInput8_DirectInput8Create()
{
	if (DetourFunction(&(PVOID&)DirectInput8Create_Original, DirectInput8Create_Hook))
		logfs("Detours: Hooked first stage sub_447A37");
	else
		logfs("Detours: Failed to hook sub_447A37");
}