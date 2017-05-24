#pragma once
typedef int(__fastcall *sub_446A37_t)(int a1);
int __fastcall sub_446A37_Hook(int a1);
void Hook_TH08_sub_447A37();

typedef HRESULT(__stdcall *DirectInput8Create_t)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);
HRESULT __stdcall DirectInput8Create_Hook(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);
void Hook_DInput8_DirectInput8Create();