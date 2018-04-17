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

	static float dot(const vec2 &a, const vec2 &b);
	static vec2 proj(const vec2 &a, const vec2 &b);
	static vec2 perp(const vec2 &a, const vec2 &b);

	/**
	 * \brief Determine the minimum component-wise vector
	 * \param a First vector
	 * \param b Second vector
	 * \return The minimum component-wise vector
	 */
	static vec2 minv(const vec2 &a, const vec2 &b);

	/**
	* \brief Determine the maximum component-wise vector
	* \param a First vector
	* \param b Second vector
	* \return the minimum component-wise vector
	*/
	static vec2 maxv(const vec2 &a, const vec2 &b);

	/**
	 * \brief Determine if p is inside an AABB defined by a and b
	 * \param p Point to test
	 * \param a Arbitrary corner of AABB
	 * \param b Arbitrary corner of AABB
	 * \return Whether p is inside an AABB defined by a and b
	 */
	static bool in_aabb(const vec2 &p, const vec2 &a, const vec2 &b);

	/**
	 * \brief Determine if AABB 1 is colliding with AABB 2
	 * \param p1 Position of AABB 1 (top-left corner)
	 * \param p2 Position of AABB 2 (top-left corner)
	 * \param s1 Size (x=width, y=height) of AABB 1
	 * \param s2 Size (x=width, y=height) of AABB 2
	 * \return Whether AABB 1 is colliding with AABB 2
	 */
	static bool is_collide_aabb(const vec2 &p1, const vec2 &p2, const vec2 &s1, const vec2 &s2);

	/**
	 * \brief Determine if AABB 2 is fully contained in AABB 1
	 * \param p1 Position of AABB 1 (top-left corner)
	 * \param p2 Position of AABB 2 (top-left corner)
	 * \param s1 Size (x=width, y=height) of AABB 1
	 * \param s2 Size (x=width, y=height) of AABB 2
	 * \return Whether AABB 2 is fully contained in AABB 1
	 */
	static bool is_contain_aabb(const vec2 &p1, const vec2 &p2, const vec2 &s1, const vec2 &s2);
	
	/**
	 * \brief Determine if AABB 1 will collide with AABB 2 in the future
	 * \param p1 Position of AABB 1 (top-left corner)
	 * \param p2 Position of AABB 2 (top-left corner)
	 * \param s1 Size (x=width, y=height) of AABB 1
	 * \param s2 Size (x=width, y=height) of AABB 2
	 * \param v1 Velocity of AABB 1 (pixels/frame)
	 * \param v2 Velocity of AABB 2 (pixels/frame)
	 * \return 0 if already collided, -1 if no collision, otherwise number of frames until collision..
	 */
	static float will_collide_aabb(const vec2 &p1, const vec2 &p2, const vec2 &s1, const vec2 &s2, 
								 const vec2 &v1, const vec2 &v2);


	/**
	 * \brief Determine if AABB 2 will exit AABB 1 in the future
	 * "Exiting" is defined as having any part of AABB 2 outside the bounds of AABB 1
	 * \param p1 Position of AABB 1 (top-left corner)
	 * \param p2 Position of AABB 2 (top-left corner)
	 * \param s1 Size (x=width, y=height) of AABB 1
	 * \param s2 Size (x=width, y=height) of AABB 2
	 * \param v1 Velocity of AABB 1 (pixels/frame)
	 * \param v2 Velocity of AABB 2 (pixels/frame)
	 * \return 0 if already exited, -1 if no exit, otherwise number of frames until exit
	 */
	static float will_exit_aabb(const vec2 &p1, const vec2 &p2, const vec2 &s1, const vec2 &s2,
							  const vec2 &v1, const vec2 &v2);

	/**
	 * \brief Determine if two circles are collided
	 * \param p1 Position of circle 1 center
	 * \param p2 Position of circle 2 center
	 * \param r1 Radius of circle 1
	 * \param r2 Radius of circle 2 
	 * \return Whether the circles are collided
	 */
	static bool is_collide_circle(const vec2 &p1, const vec2 &p2, float r1, float r2);

	/**
	 * \brief Determine if circle 1 will collide with circle 2 in the future
	 * \param p1 Position of circle 1 center
	 * \param p2 Position of circle 2 center
	 * \param r1 Radius of circle 1
	 * \param r2 Radius of circle 2
	 * \return 0 if already collided, -1 if no collision, otherwise number of frames until collision
	 */
	static float will_collide_circle(const vec2 &p1, const vec2 &p2, float r1, float r2,
								const vec2 &v1, const vec2 &v2);
	
	/**
	 * \brief Determine if a circle will collide with a line segment in the future
	 * \param ct The center of the circle
	 * \param v The velocity of the circle
	 * \param r The radius of the circle
	 * \param p1 An endpoint of the line segment
	 * \param p2 The other endpoint of the line segment
	 * \return 0 if already collided, -1 if no collision, otherwise number of frames until collision
	 */
	static float will_collide_circle_line(const vec2 &ct, const vec2 &v, float r, 
							const vec2 &p1, const vec2 &p2);
	
	/**
	 * \brief Solve the specified quadratic equation of the form 
	 * ax^2 + bx + c
	 * \param a Leading coefficient of x^2
	 * \param b 2nd coefficient of x
	 * \param c Constant term
	 * \param x1 First root
	 * \param x2 Second root
	 * \return Number of solutions (0, 1, 2)
	 */
	static int quadratic_solve(float a, float b, float c, float &x1, float &x2);
};

vec2 operator+(const vec2& a, const vec2& b);
vec2 operator-(const vec2& a, const vec2& b);
vec2 operator*(const vec2& a, float o);
vec2 operator/(const vec2& a, float o);
vec2 operator*(float o, const vec2& a);
vec2 operator/(float o, const vec2& a);