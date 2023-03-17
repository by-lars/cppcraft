#include "Platform/Zune/ZuneWindow.h"
#include "Core/Base.h"
#include <zdk.h>

namespace ZuneCraft {
	static void SuppressReboot()
	{
		HKEY key = NULL;
		HRESULT hr = S_OK;
		DWORD value;

		if (SUCCEEDED(hr))
			hr = HRESULT_FROM_WIN32(RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"System\\CurrentControlSet\\Control\\Power\\State\\Reboot", 0, 0, &key));
		if (SUCCEEDED(hr))
			hr = HRESULT_FROM_WIN32(RegSetValueEx(key, L"Flags", 0, REG_DWORD, (BYTE*)&(value = 0x10000), sizeof(DWORD)));
		if (SUCCEEDED(hr))
			hr = HRESULT_FROM_WIN32(RegSetValueEx(key, L"Default", 0, REG_DWORD, (BYTE*)&(value = 0), sizeof(DWORD)));
		if (key)
			RegCloseKey(key);
	}

	extern __inline BOOL ZDKSystem_ExitRequested()
	{
		static HANDLE evt = NULL;
		if (!evt)
		{
			evt = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"ZAM/AppExitEvent");
			if (!evt)
				return FALSE;
		}
		return (WaitForSingleObject(evt, 0) == WAIT_OBJECT_0);
	}

	ZuneWindow::ZuneWindow() {
		SuppressReboot();
		ZDKSystem_ShowSplashScreen(false);
		ZDKSystem_SetOrientation(ORIENTATION_270);
		ZDKGL_Initialize();
		
		//Height and Width are swapped for landscape mode
		ZDKDisplay_GetInfo((DWORD*)&m_Height, (DWORD*)&m_Width);
	}

	bool ZuneWindow::ShouldClose() {
		return ZDKSystem_ExitRequested();
	}

	void* ZuneWindow::GetNativeWindow() {
		ZC_ASSERT(false, "NativeWindow not implemented for Platform: Zune");
		return NULL;
	}

	void ZuneWindow::Update() {

	}

	ZuneWindow::~ZuneWindow() {
		ZDKGL_Cleanup();
	}
}