#pragma once

#ifdef CORE_BUILD_SHARED
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif

#ifdef TOOL_BUILD_SHARED
#define TOOL_API __declspec(dllexport)
#else
#define TOOL_API __declspec(dllimport)
#endif