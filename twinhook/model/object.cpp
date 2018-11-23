#include "stdafx.h"
#include "object.h"

std::shared_ptr<entity> entity::entityAtCollision(const entity &o) const
{
	float t = this->willCollideWith(o);
	if (t < 0)
		return nullptr;
	return this->translate(t * this->velocity);
}

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
		auto poly = this->toPolygon();
		return poly.willCollideWith(a);
	}
	default: return -1.f;
	}
}

polygon aabb::toPolygon() const
{
	std::vector<vec2> points = {
		position, position + vec2(size.w, 0),
		position + size, position + vec2(0, size.h)
	};
	return polygon{ points, velocity };
}

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
		const auto& a = dynamic_cast<const aabb&>(o);
		auto bounds = this->boundingBox();
		// Use bounding box of circle
		return vec2::willExitAABB(bounds->position, a.position,
			bounds->size, a.size, bounds->velocity, a.velocity);
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

vec2 polygon::com() const
{
	vec2 sum;
	for (const vec2&v : points)
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
	switch (o.type)
	{
	default: return -1.f;
	}
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
		const auto& a = dynamic_cast<const aabb&>(o);
		const auto poly = a.toPolygon();
		return this->willCollideWith(poly);
	}
	default: return -1.f;
	}
}

std::vector<vec2> obb::toVertices(vec2 position, float length, float radius, float angle)
{
	std::vector<vec2> vertices;
	// note: these vertices must be in VERTEX ORDER
	vec2 points[] = {
		vec2(0, radius), vec2(0, -radius),
		vec2(length, -radius), vec2(length, radius)
	};
	vertices.reserve(4);
	for (const auto& point : points)
		vertices.push_back(point.rotate(angle) + position);
	return vertices;
}
