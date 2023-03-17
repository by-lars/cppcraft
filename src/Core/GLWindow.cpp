#include "Core/GLWindow.h"


#ifdef ZC_PLATFORM_WIN32
#include "Platform/Windows/WindowsWindow.h"
#elif ZC_PLATFORM_ZUNE
#include "Platform/Zune/ZuneWindow.h"

#endif 


namespace ZuneCraft {
	Window* Window::Create() {
	#ifdef ZC_PLATFORM_WIN32
		return new WindowsWindow();
	#elif ZC_PLATFORM_ZUNE
		return new ZuneWindow();
	#else
	#error Unknown Platform
	#endif 
	}
}