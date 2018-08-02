#include "stdafx.h"
#include "th_patch_registry.h"
#include "patch/th08_patch_autobomb.h"
#include "th10_patch_autobomb.h"
#include "th15_patch_autobomb.h"
#include "th11_patch_dinput_fix.h"

std::unordered_map<std::string, th_patch*> th_registry::mapPatch{
	{ "th08_autobomb",  new th08_patch_autobomb() },
	{ "th10_autobomb",  new th10_patch_autobomb() },
	{ "th15_autobomb",  new th15_patch_autobomb() },
	{ "th11_dinput_fix", new th11_patch_dinput_fix()}
};

th_patch* th_registry::getPatch(const std::string& patchName)
{
	if (mapPatch.find(patchName) == mapPatch.end())
		return nullptr;
	return mapPatch[patchName];
}

th_patch::result th_registry::patch(const std::string& patchName)
{
	th_patch *p = getPatch(patchName);
	th_patch::result code;
	if (!p)
		code = th_patch::PATCH_INVALID;
	else
		code = p->patch();
	LOG("apply patch %s: result code %x", patchName.c_str(), code);
	return code;
}

th_patch::result th_registry::unpatch(const std::string& patchName)
{
	th_patch *p = getPatch(patchName);
	th_patch::result code;

	if (!p)
		code = th_patch::PATCH_INVALID;
	else
		code = p->unpatch();
	LOG("remove patch %s: result code %x", patchName.c_str(), code);
	return code;
}
