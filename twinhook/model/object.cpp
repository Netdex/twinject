#include "object.h"

float aabb::willCollideWith(const entity& o)
{
	switch (o.type)
	{
	case AABB:
		const auto& a = dynamic_cast<const aabb&>(o);
		return vec2::willCollideAABB(position, a.position,
			size, a.size, velocity, a.velocity);
	default: return -1.f;
	}
}

float circle::willCollideWith(const entity& o)
{
	switch (o.type)
	{
	case Circle:
		const auto& a = dynamic_cast<const circle&>(o);
		return vec2::willCollideCircle(center, a.center,
			radius, a.radius, velocity, a.velocity);
	default: return -1.f;
	}
}

float polygon::willCollideWith(const entity& o)
{
	switch (o.type)
	{
	case Polygon:
		const auto& a = dynamic_cast<const polygon&>(o);
		return vec2::willCollideSAT(points, velocity, a.points, a.velocity);
	default: return -1.f;
	}
}
