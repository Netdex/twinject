#pragma once

struct Direct3D9Hook;

HMODULE WINAPI LoadLibraryA_Hook(LPCSTR lpFileName);
typedef HMODULE(WINAPI *LoadLibrary_t)(LPCSTR);
void Hook_Kernel32_LoadLibraryA(Direct3D9Hook hok);