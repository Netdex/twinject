#include "polygon.h"

#include <sstream>

#include "aabb.h"

vec2 polygon::com() const
{
	vec2 sum;
	for (const vec2& v : points)
		sum += v;
	sum /= float(points.size());
	return sum;
}

std::shared_ptr<entity> polygon::translate(vec2 delta) const
{
	auto c = std::make_shared<polygon>(*this);
	for (vec2& v : c->points)
		v += delta;
	return c;
}

std::shared_ptr<entity> polygon::withVelocity(vec2 newVelocity) const
{
	auto c = std::make_shared<polygon>(*this);
	c->velocity = newVelocity;
	return c;
}

std::shared_ptr<aabb> polygon::boundingBox() const
{
	vec2 minv = vec2::minv(points);
	vec2 maxv = vec2::maxv(points);
	return std::make_shared<aabb>(minv, velocity, maxv - minv);
}

float polygon::willExit(const entity& o) const
{
	return -1.f;
}

float polygon::willCollideWith(const entity& o) const
{
	switch (o.type)
	{
	case Polygon: {
		const auto& a = dynamic_cast<const polygon&>(o);
		return vec2::willCollideSAT(points, velocity, a.points, a.velocity);
	}
	case AABB: {
		// The opposite is already defined, so use that one
		return o.willCollideWith(*this);
	}
	default: return -1.f;
	}
}

std::ostream& polygon::serialize(std::ostream& os) const
{
	os << "polygon { ";
	for (const vec2& p : points)
	{
		os << p << " ";
	}
	os << "} v" << velocity;
	return os;
}