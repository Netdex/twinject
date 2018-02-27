#include "stdafx.h"

#ifdef DEBUG
#include "CppUnitTest.h"
#include "vec2.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(Vec2Test)
{
public:
	TEST_METHOD(VectorEquality)
	{
		vec2 a(1, 1);
		vec2 b(1, 1);
		vec2 c(2, 2);
		Assert::IsTrue(a == b, L"vector equality", LINE_INFO());
		Assert::IsFalse(a == c, L"vector inequality", LINE_INFO());
	}

	TEST_METHOD(VectorIsCollideAABB)
	{
		Assert::IsTrue(
			vec2::is_collide_aabb(vec2(0, 0), vec2(1, 1), vec2(2, 2), vec2(1, 1)),
			L"aabb collides", LINE_INFO()
		);
		Assert::IsTrue(
			vec2::is_collide_aabb(vec2(0, 0), vec2(0, 1), vec2(1, 1), vec2(1, 1)),
			L"aabb touches", LINE_INFO()
		);
		Assert::IsFalse(
			vec2::is_collide_aabb(vec2(0, 0), vec2(10, 10), vec2(1, 1), vec2(1, 1)),
			L"aabb does not collide", LINE_INFO()
		);
	}

	TEST_METHOD(VectorWillCollideAABB)
	{
		// positive cases
		Assert::IsTrue(
			vec2::will_collide_aabb(
				vec2(0, 0), vec2(10, 0),		// points
				vec2(1, 1), vec2(1, 1),			// sizes
				vec2(1, 0), vec2(-1, 0)			// velocities
			) > 0,
			L"aabb will collide, converging horizontal", LINE_INFO()
		);
		Assert::IsTrue(
			vec2::will_collide_aabb(
				vec2(0, 0), vec2(10, 10),		// points
				vec2(1, 1), vec2(1, 1),			// sizes
				vec2(1, 1), vec2(-1, -1)		// velocities
			) > 0,
			L"aabb will collide, converging diagonal", LINE_INFO()
		);
		Assert::IsTrue(
			vec2::will_collide_aabb(
				vec2(0, 0), vec2(0, 10),		// points
				vec2(1, 1), vec2(1, 1),			// sizes
				vec2(0, 1), vec2(0, -1)		// velocities
			) > 0,
			L"aabb will collide, converging vertical", LINE_INFO()
		);

		// negative cases
		Assert::IsFalse(
			vec2::will_collide_aabb(
				vec2(0, 0), vec2(10, 0),		// points
				vec2(1, 1), vec2(1, 1),			// sizes
				vec2(1, 0), vec2(1, 0)			// velocities
			) > 0,
			L"aabb will not collide, parallel velocity", LINE_INFO()
		);
		Assert::IsFalse(
			vec2::will_collide_aabb(
				vec2(0, 0), vec2(10, 2),		// points
				vec2(1, 1), vec2(1, 1),			// sizes
				vec2(1, 0), vec2(-1, 0)			// velocities
			) > 0,
			L"aabb will not collide, separating axis", LINE_INFO()
		);
		Assert::IsFalse(
			vec2::will_collide_aabb(
				vec2(0, 0), vec2(10, 0),		// points
				vec2(1, 1), vec2(1, 1),			// sizes
				vec2(-1, 0), vec2(1, 0)			// velocities
			) > 0,
			L"aabb will collide, diverging horizontal", LINE_INFO()
		);
		Assert::IsFalse(
			vec2::will_collide_aabb(
				vec2(0, 0), vec2(10, 10),		// points
				vec2(1, 1), vec2(1, 1),			// sizes
				vec2(-1, -1), vec2(1, 1)		// velocities
			) > 0,
			L"aabb will collide, diverging diagonal", LINE_INFO()
		);
		Assert::IsFalse(
			vec2::will_collide_aabb(
				vec2(0, 0), vec2(0, 10),		// points
				vec2(1, 1), vec2(1, 1),			// sizes
				vec2(0, -1), vec2(0, 1)		// velocities
			) > 0,
			L"aabb will collide, diverging vertical", LINE_INFO()
		);

		// special cases
		Assert::AreEqual(
			vec2::will_collide_aabb(
				vec2(0, 0), vec2(2, 2),			// points
				vec2(3, 3), vec2(3, 3),			// sizes
				vec2(-1, -1), vec2(1, 1)		// velocities
			), 0,
			L"aabb already collided", LINE_INFO()
		);
	}
};
#endif