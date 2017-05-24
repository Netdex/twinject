#pragma once
HMODULE WINAPI LoadLibraryA_Hook(LPCSTR lpFileName);
typedef HMODULE(WINAPI *LoadLibrary_t)(LPCSTR);
void Hook_Kernel32_LoadLibraryA();