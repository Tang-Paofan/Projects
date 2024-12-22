#pragma once

#ifdef CRASHREPORTER_LIB
#define CRASHREPORTER_EXPORT __declspec(dllexport)
#else
#define CRASHREPORTER_EXPORT __declspec(dllimport)
#endif

#define CHECK_IF(cond, sts) \
    {                       \
        if (cond)           \
            return (sts);   \
    }