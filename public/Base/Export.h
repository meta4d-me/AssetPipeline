#pragma once

#ifdef TOOL_BUILD_SHARED
#define TOOL_API __declspec(dllexport)
#else
#define TOOL_API
#endif