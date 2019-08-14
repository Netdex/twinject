#pragma once
#include <spdlog/spdlog.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define ASSERT(a) do{if(!(a)){SPDLOG_CRITICAL("Assertion Failed: " TOSTRING(a)); exit(1);}}while(0)

