#pragma once

#include "Base/Template.h"

#ifdef CORE_BUILD_SHARED
#define CORE_API API_EXPORT
#else
#define CORE_API API_IMPORT
#endif

#ifdef TOOL_BUILD_SHARED
#define TOOL_API API_EXPORT
#else
#define TOOL_API API_IMPORT
#endif