#include "stdafx.h"
#include "DI8Control.h"
#include "IDI8ADevice_Wrapper.h"

BYTE diProxyKeys[256] = {0};
BOOL diKeyMask[256] = {FALSE};

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
	diKeyMask[vk] = false;
}

HRESULT DI8_GetDeviceState_Hook(DirectInputDevice8Wrapper *pDInput8, DWORD cbData, LPVOID lpvData)
{
	BYTE aData[256];
	HRESULT result = pDInput8->DirectInputDevice8->GetDeviceState(cbData, aData);
	DI8_SetData(aData, (BYTE*)lpvData);
	return result;
}
