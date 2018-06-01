#pragma once
#include "util/vec2.h"
#include "control/th_player.h"

class th_laser_expander
{
	float maxDimensionSize;
public:
	th_laser_expander(float maxDimensionSize) : maxDimensionSize(maxDimensionSize) {}

	void expand(const std::vector<laser> &lasers, std::vector<entity> &bullets) const;

private:
	void quadtreeDivide(const std::vector<vec2> &laserVertices, 
		const vec2 &p, const vec2 &s, std::vector<entity> &bullets) const;
};
