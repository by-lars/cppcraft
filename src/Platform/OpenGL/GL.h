#pragma once

#if defined(ZC_PLATFORM_WIN32) || defined(ZC_PLATFORM_LINUX)
#include <glad/gl.h>
#define GL_NVIDIA_PLATFORM_BINARY_NV 0
#endif
