#include "stdafx.h"
#include "th_registry.h"

std::unordered_map<std::string, uint32_t> th_registry::global{

};

bool th_registry::read(const std::string& key, uint32_t &retval)
{
	if (global.find(key) == global.end())
		return false;
	retval = global[key];
	return true;
}

void th_registry::write(const std::string& key, uint32_t val)
{
	global[key] = val;
}
