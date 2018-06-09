#include "../stdafx.h"
#include "th_di8_hook.h"
#include "../directx/IDI8A_Wrapper.h"
#include "../util/detour.h"
#include "../directx/IDI8ADevice_Wrapper.h"

th_di8_hook* th_di8_hook::instance = nullptr;

void th_di8_hook::bind(th_player* player)
{
	ASSERT(("cannot multi-bind", !instance));
	instance = new th_di8_hook(player);

	DirectInput8Hook di8_hk;
	di8_hk.GetDeviceStateHook = di8GetDeviceStateHook;
	Hook_DInput8_DirectInput8Create(di8_hk);
}

th_di8_hook* th_di8_hook::inst()
{
	ASSERT(("cannot obtain unbounded instance", instance));
	return instance;
}

void th_di8_hook::set_vk_data(BYTE *pData, BYTE *pActual)
{
	// if the block flag is set, make the game ignore all input
	if(block)
	{
		memset(pActual, DIK_KEY_UP, 256);
		return;
	}

	// set the proper key states, taking the mask into account
	for (int i = 0; i < 256; i++)
	{
		if (di8_key_mask[i])
			pActual[i] = di8_proxy_keys[i];
		else
			pActual[i] = pData[i];
	}
}

HRESULT th_di8_hook::di8GetDeviceStateHook(DirectInputDevice8Wrapper* lpDirectInput, DWORD cbData, LPVOID lpvData)
{
	HRESULT result = lpDirectInput->DirectInputDevice8->GetDeviceState(cbData, inst()->di8_last_keys);
	inst()->set_vk_data(inst()->di8_last_keys, (BYTE*)lpvData);
	return result;
}

BYTE th_di8_hook::getVkState(BYTE vk)
{
	if (di8_key_mask[vk])
		return di8_proxy_keys[vk];
	return di8_last_keys[vk];
}

void th_di8_hook::setVkState(BYTE vk, BYTE state)
{
	di8_proxy_keys[vk] = state;
	di8_key_mask[vk] = true;
}

void th_di8_hook::resetVkState(BYTE vk)
{
	di8_proxy_keys[vk] = 0;
	di8_key_mask[vk] = false;
}

static DirectInput8Create_t DirectInput8Create_Original = DirectInput8Create;

/**
* \brief Due to the nature of trampolined functions, we must memoize the
* hook parameters statically.
*/
static DirectInput8Hook di8_hook;

static HRESULT __stdcall DirectInput8Create_Hook(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter)
{
	LOG("DI8Create: Feeding fake DirectInput");
	LPDIRECTINPUT8A legit;
	HRESULT result = DirectInput8Create_Original(hinst, dwVersion, riidltf, (LPVOID*)&legit, punkOuter);
	th_di8_hook::inst()->DirectInput8 = new DirectInput8Wrapper(legit, di8_hook);
	*ppvOut = th_di8_hook::inst()->DirectInput8;
	return result;
}

static  void Hook_DInput8_DirectInput8Create(DirectInput8Hook hook)
{
	di8_hook = hook;
	if (DetourFunction(&(PVOID&)DirectInput8Create_Original, DirectInput8Create_Hook))
		LOG("Detours: Hooked second stage DirectInput8Create");
	else
		LOG("Detours: Failed to hook DirectInput8Create");
}