#include "stdafx.h"
#include "IDI8A_Wrapper.h"
#include "IDI8ADevice_Wrapper.h"

DirectInput8Wrapper::DirectInput8Wrapper(LPDIRECTINPUT8A pDirectInput)
{
	DirectInput8 = pDirectInput;
}

DirectInput8Wrapper::~DirectInput8Wrapper() {}

HRESULT DirectInput8Wrapper::QueryInterface(const IID& riid, LPVOID* ppvObj)
{
	return DirectInput8->QueryInterface(riid, ppvObj);
}

ULONG DirectInput8Wrapper::AddRef()
{
	return DirectInput8->AddRef();
}

ULONG DirectInput8Wrapper::Release()
{
	return DirectInput8->Release();
}

HRESULT DirectInput8Wrapper::CreateDevice(const GUID& rguid, LPDIRECTINPUTDEVICE8A* lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
{
	LPDIRECTINPUTDEVICE8A pDirectInputDevice8A;
	HRESULT hRes = DirectInput8->CreateDevice(rguid, &pDirectInputDevice8A, pUnkOuter);
	DirectInputDevice8 = pDirectInputDevice8A;
	*lplpDirectInputDevice = new DirectInputDevice8Wrapper(pDirectInputDevice8A);
	return hRes;
}

HRESULT DirectInput8Wrapper::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	return DirectInput8->EnumDevices(dwDevType, lpCallback, pvRef, dwFlags);
}

HRESULT DirectInput8Wrapper::GetDeviceStatus(const GUID& rguidInstance)
{
	return DirectInput8->GetDeviceStatus(rguidInstance);
}

HRESULT DirectInput8Wrapper::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	return DirectInput8->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT DirectInput8Wrapper::Initialize(HINSTANCE hinst, DWORD dwVersion)
{
	return DirectInput8->Initialize(hinst, dwVersion);
}

HRESULT DirectInput8Wrapper::FindDevice(const GUID& rguidClass, LPCSTR ptszName, LPGUID pguidInstance)
{
	return DirectInput8->FindDevice(rguidClass, ptszName, pguidInstance);
}

HRESULT DirectInput8Wrapper::EnumDevicesBySemantics(LPCSTR ptszUserName, LPDIACTIONFORMATA lpdiActionFormat, LPDIENUMDEVICESBYSEMANTICSCBA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	return DirectInput8->EnumDevicesBySemantics(ptszUserName, lpdiActionFormat, lpCallback, pvRef, dwFlags);
}

HRESULT DirectInput8Wrapper::ConfigureDevices(LPDICONFIGUREDEVICESCALLBACK lpdiCallback, LPDICONFIGUREDEVICESPARAMSA lpdiCDParams, DWORD dwFlags, LPVOID pvRefData)
{
	return DirectInput8->ConfigureDevices(lpdiCallback, lpdiCDParams, dwFlags, pvRefData);
}
