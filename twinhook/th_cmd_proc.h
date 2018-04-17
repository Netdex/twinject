#pragma once
#include <unordered_map>

// TODO implement command processor
class th_cmd_proc
{
	// we only support configuration of floats of course
	std::unordered_map<std::string, float> data;
};
