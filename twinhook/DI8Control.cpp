#include "stdafx.h"
#include "DI8Control.h"
#include "IDI8ADevice_Wrapper.h"

BYTE diLastKeys[256] = { 0 };
BYTE diProxyKeys[256] = { 0 };
BOOL diKeyMask[256] = { FALSE };

void DI8_SetData(BYTE *pData, BYTE *pActual)
{
	for (int i = 0; i < 256; i++)
	{
		if (diKeyMask[i])
			pActual[i] = diProxyKeys[i];
		else
			pActual[i] = pData[i];
	}
}

void DI8C_SetKeyState(BYTE vk, BYTE state)
{
	diProxyKeys[vk] = state;
	diKeyMask[vk] = true;
}

void DI8C_ResetKeyState(BYTE vk)
{
	diProxyKeys[vk] = 0;
	diKeyMask[vk] = false;
}

BYTE DI8C_GetVirtualKeyState(BYTE vk)
{
	if (diKeyMask[vk])
		return diProxyKeys[vk];
	return diLastKeys[vk];
}

HRESULT DI8_GetDeviceState_Hook(DirectInputDevice8Wrapper *pDInput8, DWORD cbData, LPVOID lpvData)
{
	HRESULT result = pDInput8->DirectInputDevice8->GetDeviceState(cbData, diLastKeys);
	DI8_SetData(diLastKeys, (BYTE*)lpvData);
	return result;
}
