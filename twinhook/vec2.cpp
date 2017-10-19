#include "stdafx.h"
#include "vec2.h"

vec2::vec2() : x(0), y(0)
{
}

vec2::vec2(float x, float y) : x(x), y(y)
{
}


vec2::vec2(const vec2& other)
	: x(other.x),
	y(other.y)
{
}

vec2::vec2(vec2&& other) noexcept
	: x(other.x),
	y(other.y)
{
}

vec2& vec2::operator=(const vec2& other)
{
	if (this == &other)
		return *this;
	x = other.x;
	y = other.y;
	return *this;
}

vec2& vec2::operator=(vec2&& other) noexcept
{
	if (this == &other)
		return *this;
	x = other.x;
	y = other.y;
	return *this;
}

bool vec2::operator==(const vec2& o) const
{
	return x == o.x && y == o.y;
}

bool vec2::operator!=(const vec2& o) const
{
	return !(*this == o);
}

vec2 operator+(const vec2& a, const vec2& b)
{
	return vec2(a.x + b.x, a.y + b.y);
}

vec2 operator-(const vec2& a, const vec2& b)
{
	return vec2(a.x - b.x, a.y - b.y);
}

vec2 operator*(const vec2& a, float o)
{
	return vec2(a.x * o, a.y * o);
}

vec2 operator/(const vec2& a, float o)
{
	return vec2(a.x / o, a.y / o);
}

vec2 operator*(float o, const vec2& a)
{
	return operator*(a, o);
}

vec2 operator/(float o, const vec2& a)
{
	return operator/(a, o);
}

void vec2::operator+=(const vec2& o)
{
	x += o.x;
	y += o.y;
}

void vec2::operator-=(const vec2& o)
{
	x -= o.x;
	y -= o.y;
}

void vec2::operator*=(float o)
{
	x *= o;
	y *= o;
}

void vec2::operator/=(float o)
{
	x /= o;
	y /= o;
}

void vec2::set(const vec2& o)
{
	x = o.x;
	y = o.y;
}

vec2 vec2::normal() const
{
	return vec2(-y, x);
}

void vec2::normalize()
{
	float l = len();
	x /= l;
	y /= l;
}

vec2 vec2::unit() const
{
	vec2 c(*this);
	c.normalize();
	return c;
}

float vec2::lensq() const
{
	return x * x + y * y;
}

float vec2::len() const
{
	return sqrt(lensq());
}

bool vec2::zero() const
{
	return abs(x) < ZERO_EPSILON && abs(y) < ZERO_EPSILON;
}

float vec2::dot(vec2& a, vec2& b)
{
	return a.x * b.x + a.y * b.y;
}

vec2 vec2::proj(vec2& a, vec2& b)
{
	return b * (dot(a, b) / b.lensq());
}

vec2 vec2::minv(vec2& a, vec2& b)
{
	return vec2(min(a.x, b.x), min(a.y, b.y));
}

vec2 vec2::maxv(vec2& a, vec2& b)
{
	return vec2(max(a.x, b.x), max(a.y, b.y));
}

bool vec2::in_aabb(vec2 &p, vec2 &a, vec2 &b)
{
	vec2 topleft = minv(a, b);
	vec2 botright = maxv(a, b);
	return p.x >= topleft.x && p.x <= botright.x && p.y >= topleft.y && p.y <= botright.y;
}
