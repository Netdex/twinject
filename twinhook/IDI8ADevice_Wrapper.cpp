#include"stdafx.h"
#include"IDI8ADevice_Wrapper.h"
#include "IDI8A_Wrapper.h"

DirectInputDevice8Wrapper::DirectInputDevice8Wrapper(LPDIRECTINPUTDEVICE8A lpDirectInputDevice, DirectInput8Wrapper *pDirectInput8)
{
	DirectInputDevice8 = lpDirectInputDevice;
	DirectInput8 = pDirectInput8;
}

DirectInputDevice8Wrapper::~DirectInputDevice8Wrapper() {}

HRESULT DirectInputDevice8Wrapper::QueryInterface(const IID&riid, LPVOID*ppvObj)
{
	HRESULT hRes = DirectInputDevice8->QueryInterface(riid, ppvObj); 
	if (hRes == S_OK)
		*ppvObj = this; 
	else
		*ppvObj = NULL;
	return hRes;
}

ULONG DirectInputDevice8Wrapper::AddRef()
{
	return DirectInputDevice8->AddRef();
}

ULONG DirectInputDevice8Wrapper::Release()
{
	return DirectInputDevice8->Release();
}

HRESULT DirectInputDevice8Wrapper::GetCapabilities(LPDIDEVCAPS lpDIDevCaps)
{
	return DirectInputDevice8->GetCapabilities(lpDIDevCaps);
}

HRESULT DirectInputDevice8Wrapper::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	return DirectInputDevice8->EnumObjects(lpCallback, pvRef, dwFlags);
}

HRESULT DirectInputDevice8Wrapper::GetProperty(REFGUID rguidProp, LPDIPROPHEADER pdiph)
{
	return DirectInputDevice8->GetProperty(rguidProp, pdiph);
}

HRESULT DirectInputDevice8Wrapper::SetProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph)
{
	return DirectInputDevice8->SetProperty(rguidProp, pdiph);
}

HRESULT DirectInputDevice8Wrapper::Acquire()
{
	return DirectInputDevice8->Acquire();
}

HRESULT DirectInputDevice8Wrapper::Unacquire()
{
	return DirectInputDevice8->Unacquire();
}

HRESULT DirectInputDevice8Wrapper::GetDeviceState(DWORD cbData, LPVOID lpvData)
{
	if (DirectInput8->Hook.GetDeviceStateHook)
		return DirectInput8->Hook.GetDeviceStateHook(this, cbData, lpvData);
	return DirectInputDevice8->GetDeviceState(cbData, lpvData);
}

HRESULT DirectInputDevice8Wrapper::GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	return DirectInputDevice8->GetDeviceData(cbObjectData, rgdod, pdwInOut, dwFlags);
}

HRESULT DirectInputDevice8Wrapper::SetDataFormat(LPCDIDATAFORMAT lpdf)
{
	return DirectInputDevice8->SetDataFormat(lpdf);
}

HRESULT DirectInputDevice8Wrapper::SetEventNotification(HANDLE hEvent)
{
	return DirectInputDevice8->SetEventNotification(hEvent);
}

HRESULT DirectInputDevice8Wrapper::SetCooperativeLevel(HWND hwnd, DWORD dwFlags)
{
	return DirectInputDevice8->SetCooperativeLevel(hwnd, dwFlags);
}

HRESULT DirectInputDevice8Wrapper::GetObjectInfo(LPDIDEVICEOBJECTINSTANCEA pdidoi, DWORD dwObj, DWORD dwHow)
{
	return DirectInputDevice8->GetObjectInfo(pdidoi, dwObj, dwHow);
}

HRESULT DirectInputDevice8Wrapper::GetDeviceInfo(LPDIDEVICEINSTANCEA pdidi)
{
	return DirectInputDevice8->GetDeviceInfo(pdidi);
}

HRESULT DirectInputDevice8Wrapper::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	return DirectInputDevice8->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT DirectInputDevice8Wrapper::Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid)
{
	return DirectInputDevice8->Initialize(hinst, dwVersion, rguid);
}

HRESULT DirectInputDevice8Wrapper::CreateEffect(REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT*ppdeff, LPUNKNOWN punkOuter)
{
	return DirectInputDevice8->CreateEffect(rguid, lpeff, ppdeff, punkOuter);
}

HRESULT DirectInputDevice8Wrapper::EnumEffects(LPDIENUMEFFECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwEffType)
{
	return DirectInputDevice8->EnumEffects(lpCallback, pvRef, dwEffType);
}

HRESULT DirectInputDevice8Wrapper::GetEffectInfo(LPDIEFFECTINFOA pdei, REFGUID rguid)
{
	return DirectInputDevice8->GetEffectInfo(pdei, rguid);
}

HRESULT DirectInputDevice8Wrapper::GetForceFeedbackState(LPDWORD pdwOut)
{
	return DirectInputDevice8->GetForceFeedbackState(pdwOut);
}

HRESULT DirectInputDevice8Wrapper::SendForceFeedbackCommand(DWORD dwFlags)
{
	return DirectInputDevice8->SendForceFeedbackCommand(dwFlags);
}

HRESULT DirectInputDevice8Wrapper::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD fl)
{
	return DirectInputDevice8->EnumCreatedEffectObjects(lpCallback, pvRef, fl);
}

HRESULT DirectInputDevice8Wrapper::Escape(LPDIEFFESCAPE pesc)
{
	return DirectInputDevice8->Escape(pesc);
}

HRESULT DirectInputDevice8Wrapper::Poll()
{
	return DirectInputDevice8->Poll();
}

HRESULT DirectInputDevice8Wrapper::SendDeviceData(DWORD cbObjectData, LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD fl)
{
	return DirectInputDevice8->SendDeviceData(cbObjectData, rgdod, pdwInOut, fl);
}

HRESULT DirectInputDevice8Wrapper::EnumEffectsInFile(LPCSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags)
{
	return DirectInputDevice8->EnumEffectsInFile(lpszFileName, pec, pvRef, dwFlags);
}

HRESULT DirectInputDevice8Wrapper::WriteEffectToFile(LPCSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags)
{
	return DirectInputDevice8->WriteEffectToFile(lpszFileName, dwEntries, rgDiFileEft, dwFlags);
}

HRESULT DirectInputDevice8Wrapper::BuildActionMap(LPDIACTIONFORMATA lpdiaf, LPCSTR lpszUserName, DWORD dwFlags)
{
	return DirectInputDevice8->BuildActionMap(lpdiaf, lpszUserName, dwFlags);
}

HRESULT DirectInputDevice8Wrapper::SetActionMap(LPDIACTIONFORMATA lpdiActionFormat, LPCSTR lptszUserName, DWORD dwFlags)
{
	return DirectInputDevice8->SetActionMap(lpdiActionFormat, lptszUserName, dwFlags);
}

HRESULT DirectInputDevice8Wrapper::GetImageInfo(LPDIDEVICEIMAGEINFOHEADERA lpdiDevImageInfoHeader)
{
	return DirectInputDevice8->GetImageInfo(lpdiDevImageInfoHeader);
}