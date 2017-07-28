#pragma once

#define ZERO_EPSILON .00001f

class vec2 {
public:
	float x, y;

	vec2();
	vec2(float x, float y);

	vec2(const vec2& other);
	vec2(vec2&& other) noexcept;
	vec2& operator=(const vec2& other);
	vec2& operator=(vec2&& other) noexcept;

	bool operator==(const vec2& o) const;
	bool operator!=(const vec2& o) const;

	void operator+=(const vec2& o);
	void operator-=(const vec2& o);
	void operator*=(float o);
	void operator/=(float o);

	void set(const vec2& o);
	vec2 normal() const;
	void normalize();
	vec2 unit() const;
	float lensq() const;
	float len() const;
	bool zero() const;
	
	static float dot(vec2 &a, vec2 &b);
	static vec2 proj(vec2 &a, vec2 &b);
};

vec2 operator+(const vec2& a, const vec2& b);
vec2 operator-(const vec2& a, const vec2& b);
vec2 operator*(const vec2& a, float o);
vec2 operator/(const vec2& a, float o);
vec2 operator*(float o, const vec2& a);
vec2 operator/(float o, const vec2& a);