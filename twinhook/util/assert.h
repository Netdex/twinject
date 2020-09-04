#pragma once
#include "stdafx.h"
#include <spdlog/spdlog.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define CHECK(x) do{if(!(x)){SPDLOG_CRITICAL("Assertion Failed: " TOSTRING(x)); exit(1);}}while(0)

