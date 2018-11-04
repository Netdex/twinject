#include "stdafx.h"
#include "th_laser_expander.h"

void th_laser_expander::expand(const std::vector<laser>& lasers, std::vector<entity>& bullets) const
{
	for(laser l : lasers)
	{
		std::vector<vec2> laserVerts = l.getVertices();;
		vec2 lmin = vec2::minv(laserVerts);
		vec2 lmax = vec2::maxv(laserVerts);
		quadtreeDivide(l, laserVerts, lmin, lmax - lmin, bullets, 0);
	}
}

void th_laser_expander::quadtreeDivide(const laser &laser, const std::vector<vec2>& laserVertices,
	const vec2& p, const vec2& s, std::vector<entity>& bullets, int splits) const
{
	if (splits >= maxSplits) return;

	std::vector<vec2> vertices = vec2::aabbVert(p, s);
	if (!vec2::isCollideSAT(laserVertices, vertices))
		return;

	if (std::max(s.x, s.y) <= maxSz || std::min(s.x, s.y) <= minSz) {
		entity newEntity = {
			p + s / 2, laser.velocity, s, 0
		};
		bullets.push_back(newEntity);
		return;
	}
	
	vec2 newSize = s / 2;

	std::vector<vec2> domains = vec2::aabbVert(p, newSize);
	for(vec2 vert : domains)
	{
		quadtreeDivide(laser, laserVertices, vert, newSize, bullets, splits + 1);
	}
}
