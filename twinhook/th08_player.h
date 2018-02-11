#pragma once
#include "th_player.h"

class th08_player : th_player
{
protected:
	~th08_player() = default;
public:
	void hook() override;
	void tick() override;
	void draw() override;
};