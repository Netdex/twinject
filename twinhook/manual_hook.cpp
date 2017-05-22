#include "stdafx.h"
#include <Windows.h>
#include <cstdio>

void print_hex(char* buf, char *arr, int len)
{

	int cnt = 0;
	for (int i = 0; i < len; i++)
		cnt += sprintf(buf + cnt, "%02x ", (unsigned char)arr[i]);
}

void __stdcall wrapper()
{
	OutputDebugString(L"test");
}

char wrapperTmpl[] = "\x60\xe8????\x61\xc3";
char patchTmpl[] = "\xe8????";

void patch(HANDLE hProcess, int addr, int size, void(__stdcall *func)())
{
	char buf[1024];
	int wrapperSize = size + 8;

	DWORD dummy;
	char* strWrapper = static_cast<char*>(VirtualAlloc(NULL, wrapperSize, MEM_COMMIT, PAGE_READWRITE));
	ReadProcessMemory(hProcess, reinterpret_cast<void*>(addr), strWrapper, size, NULL);
	memcpy(strWrapper + size, wrapperTmpl, 8);
	*reinterpret_cast<int*>(strWrapper + size + 1 + 1) = int(func) - (int(strWrapper) + size + 5 + 1);
	VirtualProtect(strWrapper, wrapperSize, PAGE_EXECUTE_READ, &dummy);

	char *strPatch = new char[size];
	memcpy(strPatch, patchTmpl, 5);
	memset(strPatch + 5, 0x90, size - 5);
	*reinterpret_cast<int*>(strPatch + 1) = int(strWrapper) - (addr + 5);

	print_hex(buf, strWrapper, wrapperSize);
	OutputDebugStringA(buf);

	print_hex(buf, strPatch, size);
	OutputDebugStringA(buf);

	WriteProcessMemory(hProcess, reinterpret_cast<void*>(addr), strPatch, 6, NULL);
}

#ifdef NEVER_DEFINED_F
typedef HMODULE(WINAPI *LoadLibrary_t)(LPCSTR);
LoadLibrary_t orig_LoadLibrary;

IDirect3D9* __stdcall hook_Direct3DCreate9(UINT sdkVers);

// The original to call
typedef IDirect3D9* (__stdcall *Direct3DCreate9_t)(UINT SDKVersion);
Direct3DCreate9_t orig_Direct3DCreate9;

// Holds address that we get in our LoadLibrary hook (used for detour)
PBYTE pDirect3DCreate9;

typedef HRESULT(APIENTRY *CreateDevice_t)(IDirect3D9*, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS*, IDirect3DDevice9**);
CreateDevice_t orig_CreateDevice;
HRESULT APIENTRY hook_CreateDevice(IDirect3D9* pInterface, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,
	DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);

DWORD* GetVtableAddress(void* pObject)
{
	// The first 4 bytes of the object is a pointer to the vtable:
	return (DWORD*)*((DWORD*)pObject);
}

PBYTE DetourFunction(PBYTE a, PVOID b)
{
	logfs << "set function detour" << std::endl;
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID*)a, b);
	DetourTransactionCommit();
	return a;
}

void HookFunction(DWORD* pVtable, void* pHookProc, void* pOldProc, int iIndex)
{
	logfs << "hooking function at index " << iIndex << std::endl;
	// Enable writing to the vtable at address we aquired
	DWORD lpflOldProtect;
	VirtualProtect((void*)&pVtable[iIndex], sizeof(DWORD), PAGE_READWRITE, &lpflOldProtect);

	// Store old address
	if (pOldProc) {
		*(DWORD*)pOldProc = pVtable[iIndex];
	}

	// Overwrite original address
	pVtable[iIndex] = (DWORD)pHookProc;

	// Restore protection
	VirtualProtect(pVtable, sizeof(DWORD), lpflOldProtect, &lpflOldProtect);
}

IDirect3D9* __stdcall hook_Direct3DCreate9(UINT sdkVers)
{
	logfs << "called direct3d create hook" << std::endl;
	IDirect3D9* pD3d9 = orig_Direct3DCreate9(sdkVers); // real one

													   // Use a vtable hook on CreateDevice to get the device pointer later
	DWORD* pVtable = GetVtableAddress(pD3d9);
	HookFunction(pVtable, (void*)&hook_CreateDevice, (void*)&orig_CreateDevice, 15);

	return pD3d9;
}

HRESULT APIENTRY hook_CreateDevice(IDirect3D9* pInterface, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
	HRESULT ret = orig_CreateDevice(pInterface, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	// Registers MUST be preserved when doing your own stuff!!
	__asm pushad

	// get a pointer to the created device 
	IDirect3DDevice9* d3ddev = *ppReturnedDeviceInterface;

	// lets log it (format in hex mode to make it easier to work with)
	char buf[50] = { 0 };
	sprintf_s(buf, sizeof(buf), "pD3ddev: %X", d3ddev);

	logfs << buf << std::endl;

	__asm popad

	return ret;
}

HMODULE WINAPI LoadLibrary_Hook(LPCSTR lpFileName)
{
	HMODULE hM = orig_LoadLibrary(lpFileName);

	logfs << "called LoadLibrary hook with lib " << lpFileName << std::endl;
	if (strcmp(lpFileName, "d3d9.dll") == 0)
	{
		// get address of function to hook
		pDirect3DCreate9 = (PBYTE)GetProcAddress(hM, "Direct3DCreate9");
		orig_Direct3DCreate9 = (Direct3DCreate9_t)DetourFunction(pDirect3DCreate9, hook_Direct3DCreate9);
	}

	return hM;
}
#endif