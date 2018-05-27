#pragma once

class DirectInputDevice8Wrapper;

typedef HRESULT(*GetDeviceState_t)(DirectInputDevice8Wrapper *lpDirectInput, DWORD cbData, LPVOID lpvData);

struct DirectInput8Hook
{
	GetDeviceState_t GetDeviceStateHook;
};

class DirectInput8Wrapper : public IDirectInput8A {
public:
	DirectInput8Wrapper(LPDIRECTINPUT8A pDirectInput, DirectInput8Hook stHook);
	virtual ~DirectInput8Wrapper();

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirectInput8A methods ***/
	STDMETHOD(CreateDevice)(THIS_ REFGUID, LPDIRECTINPUTDEVICE8A *, LPUNKNOWN);
	STDMETHOD(EnumDevices)(THIS_ DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD);
	STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD);
	STDMETHOD(FindDevice)(THIS_ REFGUID, LPCSTR, LPGUID);
	STDMETHOD(EnumDevicesBySemantics)(THIS_ LPCSTR, LPDIACTIONFORMATA, LPDIENUMDEVICESBYSEMANTICSCBA, LPVOID, DWORD);
	STDMETHOD(ConfigureDevices)(THIS_ LPDICONFIGUREDEVICESCALLBACK, LPDICONFIGUREDEVICESPARAMSA, DWORD, LPVOID);

	LPDIRECTINPUT8A DirectInput8;
	DirectInputDevice8Wrapper *DirectInputDevice8;
	DirectInput8Hook Hook;
};

