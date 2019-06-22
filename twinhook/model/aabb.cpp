#include "aabb.h"

#include <sstream>

#include "polygon.h"

vec2 aabb::com() const
{
	return position + size / 2;
}

std::shared_ptr<entity> aabb::translate(vec2 delta) const
{
	auto c = std::make_shared<aabb>(*this);
	c->position += delta;
	return c;
}

std::shared_ptr<entity> aabb::withVelocity(vec2 newVelocity) const
{
	auto c = std::make_shared<aabb>(*this);
	c->velocity = newVelocity;
	return c;
}

std::shared_ptr<aabb> aabb::boundingBox() const
{
	return std::make_shared<aabb>(*this);
}

float aabb::willExit(const entity& o) const
{
	switch (o.type)
	{
	case AABB: {
		const auto& a = dynamic_cast<const aabb&>(o);
		float t = vec2::willExitAABB(a.position, position,
			a.size, size, a.velocity, velocity);
		return t;
	}
	default: return -1.f;
	}
}

float aabb::willCollideWith(const entity& o) const
{
	switch (o.type)
	{
	case AABB: {
		const auto& a = dynamic_cast<const aabb&>(o);
		return vec2::willCollideAABB(position, a.position,
			size, a.size, velocity, a.velocity);
	}
	case Polygon: {
		const auto& a = dynamic_cast<const polygon&>(o);
		const auto poly = this->toPolygon();
		return poly.willCollideWith(a);
	}
	default: return -1.f;
	}
}

polygon aabb::toPolygon() const
{
	return polygon{ {
		position, position + vec2(size.w, 0),
		position + size, position + vec2(0, size.h)
	}, velocity };
}

std::ostream& aabb::serialize(std::ostream& os) const
{
	os << "aabb p" << position << " v" << velocity << " s" << size;
	return os;
}

