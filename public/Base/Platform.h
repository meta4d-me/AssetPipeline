#pragma once

#include "Base/Endian.h"

#ifdef _MSC_VER
#define API_EXPORT __declspec(dllexport)
#define API_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
#define API_EXPORT __attribute__((visibility("default")))
#define API_IMPORT
#else
#define API_EXPORT
#define API_IMPORT
#endif

// Inline in cpp doesn't gurantee that compiler will treat it as an inline function.
// Instead, it only suggest the compiler to treat it as an inline function.
// So we still need to use force inline in writing highly performance critical codes.
// Don't need to use it everywhere. Use it after profiling will be better.
#ifdef _MSC_VER
#	define CD_FORCEINLINE __forceinline
#	define CD_NOINLINE __declspec(noinline)
#else
#	define CD_FORCEINLINE inline __attribute__((always_inline))
#	define CD_NOINLINE __attribute__((noinline))
#endif