#pragma once

class DirectInputDevice8Wrapper;
HRESULT DI8_GetDeviceState_Hook(DirectInputDevice8Wrapper *pDInput8, DWORD cbData, LPVOID lpvData);

void DI8C_SetKeyState(BYTE vk, BYTE state);
void DI8C_ResetKeyState(BYTE vk);