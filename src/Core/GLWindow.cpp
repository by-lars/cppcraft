#include "Core/GLWindow.h"


#if defined(ZC_PLATFORM_WIN32) || defined(ZC_PLATFORM_LINUX)
#include "Platform/GLFW/GLFWWindow.h"
#elif ZC_PLATFORM_ZUNE
#include "Platform/Zune/ZuneWindow.h"

#endif 


namespace ZuneCraft {
	Window* Window::Create() {
	#if defined(ZC_PLATFORM_WIN32) || defined(ZC_PLATFORM_LINUX)
		return new GLFWWindow();
	#elif ZC_PLATFORM_ZUNE
		return new ZuneWindow();
	#else
	#error Unknown Platform
	#endif

	return nullptr;
	}

}