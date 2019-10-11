#pragma once

#if defined(_WIN32)
    #define AGZ_OS_WIN32
#elif defined(__linux)
    #define AGZ_OS_LINUX
#endif

#if defined(_MSC_VER)
    #define AGZ_CC_MSVC
#elif defined(__clang__)
    #define AGZ_CC_CLANG
    #define AGZ_CC_IS_GNU
#elif defined(__GNUC__)
    #define AGZ_CC_GCC
    #define AGZ_CC_IS_GNU
#endif

#if defined(_DEBUG) || defined(DEBUG)
    #define AGZ_DEBUG
    #define AGZ_WHEN_DEBUG(X) do { X; } while(false)
#else
    #define AGZ_WHEN_DEBUG(X) do { } while(false)
#endif
