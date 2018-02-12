#include "stdafx.h"
#include "th_di8_hook.h"
#include "IDI8A_Wrapper.h"
#include "detour.h"
#include "IDI8ADevice_Wrapper.h"

void th_di8_hook::hook()
{
	DirectInput8Hook di8_hk;
	di8_hk.GetDeviceStateHook = di8_get_device_state_hook;
	Hook_DInput8_DirectInput8Create(di8_hk);
}

DirectInput8Wrapper *DirectInput8 = nullptr;
static DirectInput8Create_t DirectInput8Create_Original = DirectInput8Create;

static BYTE DI8Control_LastKeys[256] = { 0 };
static BYTE DI8Control_ProxyKeys[256] = { 0 };
static BOOL DI8Control_KeyMask[256] = { FALSE };

void th_di8_hook::set_vk_data(BYTE *pData, BYTE *pActual)
{
	for (int i = 0; i < 256; i++)
	{
		if (DI8Control_KeyMask[i])
			pActual[i] = DI8Control_ProxyKeys[i];
		else
			pActual[i] = pData[i];
	}
}

HRESULT th_di8_hook::di8_get_device_state_hook(DirectInputDevice8Wrapper* lpDirectInput, DWORD cbData, LPVOID lpvData)
{
	HRESULT result = lpDirectInput->DirectInputDevice8->GetDeviceState(cbData, DI8Control_LastKeys);
	set_vk_data(DI8Control_LastKeys, (BYTE*)lpvData);
	return result;
}

DirectInput8Wrapper* th_di8_hook::get_di8w()
{
	return DirectInput8;
}

BYTE th_di8_hook::get_vk_state(BYTE vk)
{
	if (DI8Control_KeyMask[vk])
		return DI8Control_ProxyKeys[vk];
	return DI8Control_LastKeys[vk];
}

void th_di8_hook::set_vk_state(BYTE vk, BYTE state)
{
	DI8Control_ProxyKeys[vk] = state;
	DI8Control_KeyMask[vk] = true;
}

void th_di8_hook::reset_vk_state(BYTE vk)
{
	DI8Control_ProxyKeys[vk] = 0;
	DI8Control_KeyMask[vk] = false;
}

/**
* \brief Due to the nature of trampolined functions, we must memoize the
* hook parameters statically.
*/
static DirectInput8Hook di8_hook;

HRESULT __stdcall DirectInput8Create_Hook(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter)
{
	LOG("DI8Create: Feeding fake DirectInput");
	LPDIRECTINPUT8A legit;
	HRESULT result = DirectInput8Create_Original(hinst, dwVersion, riidltf, (LPVOID*)&legit, punkOuter);
	DirectInput8 = new DirectInput8Wrapper(legit, di8_hook);
	*ppvOut = DirectInput8;
	return result;
}

void Hook_DInput8_DirectInput8Create(DirectInput8Hook hook)
{
	di8_hook = hook;
	if (DetourFunction(&(PVOID&)DirectInput8Create_Original, DirectInput8Create_Hook))
		LOG("Detours: Hooked second stage DirectInput8Create");
	else
		LOG("Detours: Failed to hook DirectInput8Create");
}