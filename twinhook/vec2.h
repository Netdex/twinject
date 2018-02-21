#pragma once

#define ZERO_EPSILON .000001f

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
	bool nan() const;
	vec2 transform(float(*t)(float)) const;

	static float dot(vec2 &a, vec2 &b);
	static vec2 proj(vec2 &a, vec2 &b);

	/**
	 * \brief Determine the minimum component-wise vector
	 * \param a First vector
	 * \param b Second vector
	 * \return The minimum component-wise vector
	 */
	static vec2 minv(vec2 &a, vec2 &b);

	/**
	* \brief Determine the maximum component-wise vector
	* \param a First vector
	* \param b Second vector
	* \return the minimum component-wise vector
	*/
	static vec2 maxv(vec2 &a, vec2 &b);

	/**
	 * \brief Determine if p is inside an AABB defined by a and b
	 * \param p Point to test
	 * \param a Arbitrary corner of AABB
	 * \param b Arbitrary corner of AABB
	 * \return If p is inside an AABB defined by a and b
	 */
	static bool in_aabb(vec2 &p, vec2 &a, vec2 &b);
};

vec2 operator+(const vec2& a, const vec2& b);
vec2 operator-(const vec2& a, const vec2& b);
vec2 operator*(const vec2& a, float o);
vec2 operator/(const vec2& a, float o);
vec2 operator*(float o, const vec2& a);
vec2 operator/(float o, const vec2& a);