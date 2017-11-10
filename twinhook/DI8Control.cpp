#include "stdafx.h"
#include "DI8Control.h"
#include "IDI8ADevice_Wrapper.h"

BYTE DI8Control_LastKeys[256] = { 0 };
BYTE DI8Control_ProxyKeys[256] = { 0 };
BOOL DI8Control_KeyMask[256] = { FALSE };

void DI8_SetData(BYTE *pData, BYTE *pActual)
{
	for (int i = 0; i < 256; i++)
	{
		if (DI8Control_KeyMask[i])
			pActual[i] = DI8Control_ProxyKeys[i];
		else
			pActual[i] = pData[i];
	}
}

void DI8C_SetKeyState(BYTE vk, BYTE state)
{
	DI8Control_ProxyKeys[vk] = state;
	DI8Control_KeyMask[vk] = true;
}

void DI8C_ResetKeyState(BYTE vk)
{
	DI8Control_ProxyKeys[vk] = 0;
	DI8Control_KeyMask[vk] = false;
}

BYTE DI8C_GetVirtualKeyState(BYTE vk)
{
	if (DI8Control_KeyMask[vk])
		return DI8Control_ProxyKeys[vk];
	return DI8Control_LastKeys[vk];
}

HRESULT DI8_GetDeviceState_Hook(DirectInputDevice8Wrapper *pDInput8, DWORD cbData, LPVOID lpvData)
{
	HRESULT result = pDInput8->DirectInputDevice8->GetDeviceState(cbData, DI8Control_LastKeys);
	DI8_SetData(DI8Control_LastKeys, (BYTE*)lpvData);
	return result;
}
