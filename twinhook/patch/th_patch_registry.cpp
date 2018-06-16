#include "stdafx.h"
#include "th_patch_registry.h"
#include "patch/th08_patch_autobomb.h"
#include "th10_patch_autobomb.h"
#include "th15_patch_autobomb.h"

std::unordered_map<std::string, th_patch*> th_registry::patch{
	{ "th08_autobomb",  new th08_patch_autobomb() },
	{ "th10_autobomb",  new th10_patch_autobomb() },
	{ "th15_autobomb",  new th15_patch_autobomb() }
};