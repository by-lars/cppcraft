#pragma once

#ifdef ZC_PLATFORM_WIN32
#include <glad/gl.h>
#elif ZC_PLATFORM_ZUNE
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif