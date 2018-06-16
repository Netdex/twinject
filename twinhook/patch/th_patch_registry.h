#pragma once
#include <string>
#include <unordered_map>
#include "patch/th_patch.h"

namespace th_registry {
	extern std::unordered_map<std::string, th_patch*> patch;
}
