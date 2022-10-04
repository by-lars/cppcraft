#pragma once

#ifdef ZC_PLATFORM_WIN32 
#pragma warning (disable : 26812)
#define ZC_ENUM enum class
#define ZC_ABORT() abort()
#endif

#ifdef ZC_PLATFORM_ZUNE
#define ZC_ENUM enum
#define nullptr NULL
#pragma warning (disable : 4482)
#define ZC_ABORT() exit(-1)
#endif