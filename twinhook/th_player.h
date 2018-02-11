#pragma once
#include <unordered_map>
#include "data.h"

class th_player
{
protected:
	~th_player() = default;
private:
	std::unordered_map<std::string, data> state;

public:
	th_player() {}
	virtual void hook() = 0;
	virtual void tick() = 0;
	virtual void draw() = 0;
};
