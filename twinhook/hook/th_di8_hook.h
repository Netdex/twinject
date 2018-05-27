#pragma once
#include "th_hook.h"
#include "../control/th_player.h"
#include "../directx/IDI8A_Wrapper.h"

class DirectInputDevice8Wrapper;

struct DirectInput8Hook;

typedef HRESULT(__stdcall *DirectInput8Create_t)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);
HRESULT __stdcall DirectInput8Create_Hook(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);
void Hook_DInput8_DirectInput8Create(DirectInput8Hook hook);

#define DIK_KEY_UP		0x0
#define DIK_KEY_DOWN	0x80

/**
 * \brief Function hook for statically loaded DirectInput8.
 * Hooking must be done immediately after DLL attach, or else
 * DirectInput8Create may be called already.
 */
class th_di8_hook : th_hook
{
	static th_di8_hook *instance;

	BYTE di8_last_keys[256] = { 0 };
	BYTE di8_proxy_keys[256] = { 0 };
	BOOL di8_key_mask[256] = { FALSE };

	void set_vk_data(BYTE *pData, BYTE *pActual);
public:
	DirectInput8Wrapper *DirectInput8 = nullptr;

	th_di8_hook(th_player *player) : th_hook(player) {}
	virtual ~th_di8_hook() = default;

	static void bind(th_player *player);
	static th_di8_hook* inst();

	BYTE get_vk_state(BYTE vk);
	void set_vk_state(BYTE vk, BYTE state);
	void reset_vk_state(BYTE vk);

	static HRESULT di8_get_device_state_hook(DirectInputDevice8Wrapper *lpDirectInput, DWORD cbData, LPVOID lpvData);
};
