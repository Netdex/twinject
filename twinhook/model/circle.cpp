#include "stdafx.h"
#include "circle.h"
#include "aabb.h"

vec2 circle::com() const
{
	return center;
}

std::shared_ptr<entity> circle::translate(vec2 delta) const
{
	auto c = std::make_shared<circle>(*this);
	c->center += delta;
	return c;
}

std::shared_ptr<entity> circle::withVelocity(vec2 newVelocity) const
{
	auto c = std::make_shared<circle>(*this);
	c->velocity = newVelocity;
	return c;
}

std::shared_ptr<aabb> circle::boundingBox() const
{
	return std::make_shared<aabb>(center - vec2(radius),
		velocity, vec2(radius) * 2);
}

float circle::willExit(const entity& o) const
{
	switch (o.type)
	{
	case AABB: {
		// Use bounding box of circle
		return this->boundingBox()->willExit(o);
	}
	default: return -1.f;
	}
}

float circle::willCollideWith(const entity& o) const
{
	switch (o.type)
	{
	case Circle: {
		const auto& a = dynamic_cast<const circle&>(o);
		return vec2::willCollideCircle(center, a.center,
			radius, a.radius, velocity, a.velocity);
	}
	default: return -1.f;
	}
}

std::ostream& circle::serialize(std::ostream& os) const
{
	os << "circle c" << center << " v" << velocity << " r" << radius;
	return os;
}
