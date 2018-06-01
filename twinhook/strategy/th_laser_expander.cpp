#include "stdafx.h"
#include "th_laser_expander.h"

void th_laser_expander::expand(const std::vector<laser>& lasers, std::vector<entity>& bullets) const
{
	for(laser l : lasers)
	{
		std::vector<vec2> laserVerts;
		l.getVertices(laserVerts);
		vec2 lmin = vec2::minv(laserVerts);
		vec2 lmax = vec2::maxv(laserVerts);
		quadtreeDivide(laserVerts, lmin, lmax - lmin, bullets);
	}
}

void th_laser_expander::quadtreeDivide(const std::vector<vec2>& laserVertices, 
	const vec2& p, const vec2& s, std::vector<entity>& bullets) const
{
	std::vector<vec2> vertices;
	vec2::aabbVert(p, s, vertices);
	if (!vec2::isCollideConvexPolygon(laserVertices, vertices))
		return;

	if (std::max(s.x, s.y) <= maxDimensionSize) {
		entity newEntity = {
			p + s / 2, vec2(), s, 0
		};
		bullets.push_back(newEntity);
		return;
	}
	
	
	vec2 newSize = s / 2;

	std::vector<vec2> domains;
	vec2::aabbVert(p, newSize, domains);
	for(vec2 v : domains)
	{
		quadtreeDivide(laserVertices, v, newSize, bullets);
	}
}
