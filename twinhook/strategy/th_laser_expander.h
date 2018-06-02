#pragma once
#include "util/vec2.h"
#include "control/th_player.h"

class th_laser_expander
{
	float minSz;
	float maxSz;
	float maxSplits;
public:
	th_laser_expander(float minDimensionSize, float maxDimensionSize, float maxSplits) 
		: minSz(minDimensionSize), maxSz(maxDimensionSize), maxSplits(maxSplits) {}

	void expand(const std::vector<laser> &lasers, std::vector<entity> &bullets) const;

private:
	void quadtreeDivide(const laser &laser, const std::vector<vec2> &laserVertices,
		const vec2 &p, const vec2 &s, std::vector<entity> &bullets, int splits) const;
};
