#pragma once
#include <string>
#include <unordered_map>
#include "patch/th_patch.h"

namespace th_registry {
	extern std::unordered_map<std::string, th_patch*> mapPatch;

	th_patch* getPatch(const std::string &patchName);
	th_patch::result patch(const std::string &patchName);
	th_patch::result unpatch(const std::string &patchName);

}
