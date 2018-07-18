#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace th_registry
{
	extern std::unordered_map<std::string, uint32_t> global;
	
	bool read(const std::string &key, uint32_t &retval);
	void write(const std::string &key, uint32_t val);
}
