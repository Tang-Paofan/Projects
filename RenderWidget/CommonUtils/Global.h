#pragma once

#ifdef COMMONUTILS
#define COMMONUTILS_EXPORT __declspec(dllexport)
#else
#define COMMONUTILS_EXPORT
#endif