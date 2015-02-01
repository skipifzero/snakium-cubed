#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "sfz/math/Vector.hpp"
#include "sfz/math/Matrix.hpp"
#include "sfz/math/MatrixSupport.hpp"

#include <unordered_map>
#include <type_traits>

TEST_CASE("Constructors", "[sfz::Matrix]")
{
	SECTION("Initializer list constructor") {
		sfz::mat3i m1{{1, 2, 3},
		              {4, 5, 6},
		              {7, 8, 9}};

		REQUIRE(m1.at(0, 0) == 1);
		REQUIRE(m1.at(0, 1) == 2);
		REQUIRE(m1.at(0, 2) == 3);
		REQUIRE(m1.at(1, 0) == 4);
		REQUIRE(m1.at(1, 1) == 5);
		REQUIRE(m1.at(1, 2) == 6);
		REQUIRE(m1.at(2, 0) == 7);
		REQUIRE(m1.at(2, 1) == 8);
		REQUIRE(m1.at(2, 2) == 9);

		REQUIRE(m1.mElements[0][0] == 1);
		REQUIRE(m1.mElements[0][1] == 4);
		REQUIRE(m1.mElements[0][2] == 7);
		REQUIRE(m1.mElements[1][0] == 2);
		REQUIRE(m1.mElements[1][1] == 5);
		REQUIRE(m1.mElements[1][2] == 8);
		REQUIRE(m1.mElements[2][0] == 3);
		REQUIRE(m1.mElements[2][1] == 6);
		REQUIRE(m1.mElements[2][2] == 9);

		sfz::mat2i m2{{1}};

		REQUIRE(m2.at(0, 0) == 1);
		REQUIRE(m2.at(0, 1) == 0);
		REQUIRE(m2.at(1, 0) == 0);
		REQUIRE(m2.at(1, 1) == 0);

		REQUIRE(m2.mElements[0][0] == 1);
		REQUIRE(m2.mElements[0][1] == 0);
		REQUIRE(m2.mElements[1][0] == 0);
		REQUIRE(m2.mElements[1][1] == 0);

		sfz::Matrix<int, 2, 3> m3{{1, 2, 3}, {4, 5, 6}};

		REQUIRE(m3.at(0, 0) == 1);
		REQUIRE(m3.at(0, 1) == 2);
		REQUIRE(m3.at(0, 2) == 3);
		REQUIRE(m3.at(1, 0) == 4);
		REQUIRE(m3.at(1, 1) == 5);
		REQUIRE(m3.at(1, 2) == 6);

		REQUIRE(m3.mElements[0][0] == 1);
		REQUIRE(m3.mElements[0][1] == 4);
		REQUIRE(m3.mElements[1][0] == 2);
		REQUIRE(m3.mElements[1][1] == 5);
		REQUIRE(m3.mElements[2][0] == 3);
		REQUIRE(m3.mElements[2][1] == 6);

		sfz::Matrix<int, 3, 2> m4{{1, 0}, {0, 1}, {0, 0}};

		REQUIRE(m4.at(0, 0) == 1);
		REQUIRE(m4.at(0, 1) == 0);
		REQUIRE(m4.at(1, 0) == 0);
		REQUIRE(m4.at(1, 1) == 1);
		REQUIRE(m4.at(2, 0) == 0);
		REQUIRE(m4.at(2, 1) == 0);

		REQUIRE(m4.mElements[0][0] == 1);
		REQUIRE(m4.mElements[0][1] == 0);
		REQUIRE(m4.mElements[0][2] == 0);
		REQUIRE(m4.mElements[1][0] == 0);
		REQUIRE(m4.mElements[1][1] == 1);
		REQUIRE(m4.mElements[1][2] == 0);
	}
}

TEST_CASE("glPtr()", "[sfz::Matrix]") {
	sfz::mat2i m1{{1, 2},
	              {3, 4}};
	const sfz::mat2i m2{{-1, -2},
	                    {-3, -4}};
	
	int* ptr1 = m1.glPtr();
	const int* ptr2 = m2.glPtr();

	REQUIRE(ptr1[0] == 1);
	REQUIRE(ptr1[1] == 3);
	REQUIRE(ptr1[2] == 2);
	REQUIRE(ptr1[3] == 4);

	REQUIRE(ptr2[0] == -1);
	REQUIRE(ptr2[1] == -3);
	REQUIRE(ptr2[2] == -2);
	REQUIRE(ptr2[3] == -4);
}

TEST_CASE("Element-wise multiplication", "[sfz::Matrix]")
{
	sfz::mat2i m1{{1, 2},
	              {3, 4}};
	sfz::mat2i m2{{1, 2},
	              {3, 4}};

	auto res1 = m1.elemMult(m2);
	auto res2 = m2.elemMult(m1);

	REQUIRE(res1 == res2);
	REQUIRE(res1.at(0, 0) == 1);
	REQUIRE(res1.at(0, 1) == 4);
	REQUIRE(res1.at(1, 0) == 9);
	REQUIRE(res1.at(1, 1) == 16);
}

TEST_CASE("Transpose", "[sfz::Matrix]")
{
	sfz::mat2i m1{{1, 2},
	              {3, 4}};
	sfz::Matrix<int, 2, 3> m2{{1, 2, 3},
	                          {4, 5, 6}};

	sfz::mat2i res1 = m1.transpose();
	REQUIRE(res1 != m1);
	REQUIRE(res1.transpose() == m1);

	REQUIRE(res1.at(0, 0) == 1);
	REQUIRE(res1.at(0, 1) == 3);
	REQUIRE(res1.at(1, 0) == 2);
	REQUIRE(res1.at(1, 1) == 4);

	sfz::Matrix<int, 3, 2> res2 = m2.transpose();
	REQUIRE(res2.transpose() == m2);

	REQUIRE(res2.at(0, 0) == 1);
	REQUIRE(res2.at(0, 1) == 4);
	REQUIRE(res2.at(1, 0) == 2);
	REQUIRE(res2.at(1, 1) == 5);
	REQUIRE(res2.at(2, 0) == 3);
	REQUIRE(res2.at(2, 1) == 6);
}

TEST_CASE("Arhitmetic & assignment operators", "[sfz::Matrix]")
{
	sfz::mat2i m1{{1, 2},
	              {3, 4}};
	sfz::mat2i m2{{1, 2},
	              {3, 4}};
	sfz::mat2i m3{{-2, -1},
	              {3, 33}};

	SECTION("+=") {
		m1 += m2;
		m2 += m3;

		REQUIRE(m1.at(0, 0) == 2);
		REQUIRE(m1.at(0, 1) == 4);
		REQUIRE(m1.at(1, 0) == 6);
		REQUIRE(m1.at(1, 1) == 8);

		REQUIRE(m2.at(0, 0) == -1);
		REQUIRE(m2.at(0, 1) == 1);
		REQUIRE(m2.at(1, 0) == 6);
		REQUIRE(m2.at(1, 1) == 37);
	}
	SECTION("-=") {
		m1 -= m2;
		m2 -= m3;

		REQUIRE(m1.at(0, 0) == 0);
		REQUIRE(m1.at(0, 1) == 0);
		REQUIRE(m1.at(1, 0) == 0);
		REQUIRE(m1.at(1, 1) == 0);

		REQUIRE(m2.at(0, 0) == 3);
		REQUIRE(m2.at(0, 1) == 3);
		REQUIRE(m2.at(1, 0) == 0);
		REQUIRE(m2.at(1, 1) == -29);
	}
	SECTION("*= (scalar)") {
		m1 *= 2;
		REQUIRE(m1.at(0, 0) == 2);
		REQUIRE(m1.at(0, 1) == 4);
		REQUIRE(m1.at(1, 0) == 6);
		REQUIRE(m1.at(1, 1) == 8);	

		m3 *= -1;
		REQUIRE(m3.at(0, 0) == 2);
		REQUIRE(m3.at(0, 1) == 1);
		REQUIRE(m3.at(1, 0) == -3);
		REQUIRE(m3.at(1, 1) == -33);
	}
	SECTION("*= (matrix of same size)") {
		sfz::mat2i m4{{1, 0}, {0, 1}};
		auto m1cpy = m1;
		m1cpy *= m4;

		REQUIRE(m1cpy.at(0, 0) == 1);
		REQUIRE(m1cpy.at(0, 1) == 2);
		REQUIRE(m1cpy.at(1, 0) == 3);
		REQUIRE(m1cpy.at(1, 1) == 4);

		m4 *= m1;
		REQUIRE(m4.at(0, 0) == 1);
		REQUIRE(m4.at(0, 1) == 2);
		REQUIRE(m4.at(1, 0) == 3);
		REQUIRE(m4.at(1, 1) == 4);
	}
}

TEST_CASE("Arhitmetic operators", "[sfz::Matrix]")
{
	sfz::mat2i m1{{1, 2}, {3, 4}};
	sfz::mat2i m2{{0, 1}, {0, 0}};
	sfz::Matrix<int, 2, 3> m3{{1, 2, 3}, {4, 5, 6}};
	sfz::Matrix<int, 3, 2> m4{{1, 0}, {0, 1}, {0, 0}};

	SECTION("+") {
		auto res1 = m1 + m2;
		auto res2 = m2 + m1;

		REQUIRE(res1 == res2);
		REQUIRE(res1.at(0, 0) == 1);
		REQUIRE(res1.at(0, 1) == 3);
		REQUIRE(res1.at(1, 0) == 3);
		REQUIRE(res1.at(1, 1) == 4);

		auto res3 = m3 + m3;
		REQUIRE(res3.at(0, 0) == 2);
		REQUIRE(res3.at(0, 1) == 4);
		REQUIRE(res3.at(0, 2) == 6);
		REQUIRE(res3.at(1, 0) == 8);
		REQUIRE(res3.at(1, 1) == 10);
		REQUIRE(res3.at(1, 2) == 12);
	}
	SECTION("-") {
		auto res1 = m1 - m2;
		auto res2 = m2 - m1;

		REQUIRE(res1 != res2);
		
		REQUIRE(res1.at(0, 0) == 1);
		REQUIRE(res1.at(0, 1) == 1);
		REQUIRE(res1.at(1, 0) == 3);
		REQUIRE(res1.at(1, 1) == 4);
		
		REQUIRE(res2.at(0, 0) == -1);
		REQUIRE(res2.at(0, 1) == -1);
		REQUIRE(res2.at(1, 0) == -3);
		REQUIRE(res2.at(1, 1) == -4);
	}
	SECTION("- (negation)") {
		auto res1 = -m1;

		REQUIRE(res1.at(0, 0) == -1);
		REQUIRE(res1.at(0, 1) == -2);
		REQUIRE(res1.at(1, 0) == -3);
		REQUIRE(res1.at(1, 1) == -4);
	}
	SECTION("* (matrix)") {
		auto res1 = m1*m2;
		REQUIRE(res1.at(0, 0) == 0);
		REQUIRE(res1.at(0, 1) == 1);
		REQUIRE(res1.at(1, 0) == 0);
		REQUIRE(res1.at(1, 1) == 3);

		auto res2 = m2*m1;
		REQUIRE(res2.at(0, 0) == 3);
		REQUIRE(res2.at(0, 1) == 4);
		REQUIRE(res2.at(1, 0) == 0);
		REQUIRE(res2.at(1, 1) == 0);

		sfz::mat2i res3 = m3*m4;
		REQUIRE(res3.at(0, 0) == 1);
		REQUIRE(res3.at(0, 1) == 2);
		REQUIRE(res3.at(1, 0) == 4);
		REQUIRE(res3.at(1, 1) == 5);
	}
	SECTION("* (vector)") {
		sfz::vec2i v1{1, -2};

		sfz::vec2i res1 = m1*v1;
		REQUIRE(res1[0] == -3);
		REQUIRE(res1[1] == -5);

		sfz::vec3i res2 = m4*v1;
		REQUIRE(res2[0] == 1);
		REQUIRE(res2[1] == -2);
		REQUIRE(res2[2] == 0);
	}
	SECTION("* (scalar)") {
		auto res1 = m1*2;
		REQUIRE(res1.at(0, 0) == 2);
		REQUIRE(res1.at(0, 1) == 4);
		REQUIRE(res1.at(1, 0) == 6);
		REQUIRE(res1.at(1, 1) == 8);

		auto res2 = -1*m2;
		REQUIRE(res2.at(0, 0) == 0);
		REQUIRE(res2.at(0, 1) == -1);
		REQUIRE(res2.at(1, 0) == 0);
		REQUIRE(res2.at(1, 1) == 0);
	}
}

TEST_CASE("Comparison operators", "[sfz::Matrix]")
{
	sfz::mat2i m1{{1, 2},
	              {3, 4}};
	sfz::mat2i m2{{1, 2},
	              {3, 4}};
	sfz::mat2i m3{{-2, -1},
	              {3, 33}};

	SECTION("==") {
		REQUIRE(m1 == m2);
		REQUIRE(m2 == m1);
		REQUIRE(!(m1 == m3));
		REQUIRE(!(m3 == m1));
		REQUIRE(!(m2 == m3));
		REQUIRE(!(m3 == m2));
	}
	SECTION("!=") {
		REQUIRE(m1 != m3);
		REQUIRE(m3 != m1);
		REQUIRE(m2 != m3);
		REQUIRE(m3 != m2);
		REQUIRE(!(m1 != m2));
		REQUIRE(!(m2 != m1));
	}
}

TEST_CASE("Hashing", "[sfz::Matrix]")
{
	sfz::mat2i m1{{2, 100},
	              {1, -99}};
	sfz::mat2i m2{{-1, 0},
	              {3, -10}};
	sfz::mat2i m3{{0, -9},
	              {32, 14}};

	// This test checks if unordered_map works as it should. Not a very good test, but the best I
	// can come up with to test if hashing works as it should at the moment.
	std::unordered_map<sfz::mat2i, int> hashMap;
	hashMap[m1] = 1;
	hashMap[m2] = 2;
	hashMap[m3] = 3;
	REQUIRE(hashMap[m1] == 1);
	REQUIRE(hashMap[m2] == 2);
	REQUIRE(hashMap[m3] == 3);

	REQUIRE(m1.hash() != m2.hash());
	REQUIRE(m2.hash() != m3.hash());
}

TEST_CASE("Is proper POD", "[sfz::Matrix]")
{
	REQUIRE(std::is_trivially_default_constructible<sfz::mat4f>::value);
	REQUIRE(std::is_trivially_copyable<sfz::mat4f>::value);
	REQUIRE(std::is_trivial<sfz::mat4f>::value);
	REQUIRE(std::is_standard_layout<sfz::mat4f>::value);
#ifndef _MSC_VER
	REQUIRE(std::is_pod<sfz::mat4f>::value);
#endif
	REQUIRE(std::is_literal_type<sfz::mat4f>::value);
}


// MatrixSupport.hpp
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool approxEqual(float lhs, float rhs)
{
	float eps = 0.001f;
	return lhs <= rhs + eps && lhs >= rhs - eps;
}

bool approxEqual(const sfz::mat4f& lhs, const sfz::mat4f& rhs)
{
	if (!approxEqual(lhs.at(0, 0), rhs.at(0, 0))) return false;
	if (!approxEqual(lhs.at(0, 1), rhs.at(0, 1))) return false;
	if (!approxEqual(lhs.at(0, 2), rhs.at(0, 2))) return false;
	if (!approxEqual(lhs.at(0, 3), rhs.at(0, 3))) return false;

	if (!approxEqual(lhs.at(1, 0), rhs.at(1, 0))) return false;
	if (!approxEqual(lhs.at(1, 1), rhs.at(1, 1))) return false;
	if (!approxEqual(lhs.at(1, 2), rhs.at(1, 2))) return false;
	if (!approxEqual(lhs.at(1, 3), rhs.at(1, 3))) return false;

	if (!approxEqual(lhs.at(2, 0), rhs.at(2, 0))) return false;
	if (!approxEqual(lhs.at(2, 1), rhs.at(2, 1))) return false;
	if (!approxEqual(lhs.at(2, 2), rhs.at(2, 2))) return false;
	if (!approxEqual(lhs.at(2, 3), rhs.at(2, 3))) return false;

	if (!approxEqual(lhs.at(3, 0), rhs.at(3, 0))) return false;
	if (!approxEqual(lhs.at(3, 1), rhs.at(3, 1))) return false;
	if (!approxEqual(lhs.at(3, 2), rhs.at(3, 2))) return false;
	if (!approxEqual(lhs.at(3, 3), rhs.at(3, 3))) return false;
	
	return true;
}

TEST_CASE("Rotation matrices", "[sfz::MatrixSupport")
{
	sfz::vec4f v1{1, 1, 1, 1};

	SECTION("xRotationMatrix()") {
		sfz::mat4f xRot90 = sfz::xRotationMatrix(sfz::g_PI_FLOAT/2);
		REQUIRE(approxEqual(xRot90, sfz::rotationMatrix(sfz::vec3f{1,0,0}, sfz::g_PI_FLOAT/2)));

		REQUIRE(approxEqual(xRot90.at(0, 0), 1));
		REQUIRE(approxEqual(xRot90.at(0, 1), 0));
		REQUIRE(approxEqual(xRot90.at(0, 2), 0));
		REQUIRE(approxEqual(xRot90.at(0, 3), 0));

		REQUIRE(approxEqual(xRot90.at(1, 0), 0));
		REQUIRE(approxEqual(xRot90.at(1, 1), 0));
		REQUIRE(approxEqual(xRot90.at(1, 2), -1));
		REQUIRE(approxEqual(xRot90.at(1, 3), 0));

		REQUIRE(approxEqual(xRot90.at(2, 0), 0));
		REQUIRE(approxEqual(xRot90.at(2, 1), 1));
		REQUIRE(approxEqual(xRot90.at(2, 2), 0));
		REQUIRE(approxEqual(xRot90.at(2, 3), 0));

		REQUIRE(approxEqual(xRot90.at(3, 0), 0));
		REQUIRE(approxEqual(xRot90.at(3, 1), 0));
		REQUIRE(approxEqual(xRot90.at(3, 2), 0));
		REQUIRE(approxEqual(xRot90.at(3, 3), 1));

		sfz::mat4f xRot180 = sfz::xRotationMatrix(sfz::g_PI_FLOAT);
		REQUIRE(approxEqual(xRot180, sfz::rotationMatrix(sfz::vec3f{1,0,0}, sfz::g_PI_FLOAT)));

		REQUIRE(approxEqual(xRot180.at(0, 0), 1));
		REQUIRE(approxEqual(xRot180.at(0, 1), 0));
		REQUIRE(approxEqual(xRot180.at(0, 2), 0));
		REQUIRE(approxEqual(xRot180.at(0, 3), 0));

		REQUIRE(approxEqual(xRot180.at(1, 0), 0));
		REQUIRE(approxEqual(xRot180.at(1, 1), -1));
		REQUIRE(approxEqual(xRot180.at(1, 2), 0));
		REQUIRE(approxEqual(xRot180.at(1, 3), 0));

		REQUIRE(approxEqual(xRot180.at(2, 0), 0));
		REQUIRE(approxEqual(xRot180.at(2, 1), 0));
		REQUIRE(approxEqual(xRot180.at(2, 2), -1));
		REQUIRE(approxEqual(xRot180.at(2, 3), 0));

		REQUIRE(approxEqual(xRot180.at(3, 0), 0));
		REQUIRE(approxEqual(xRot180.at(3, 1), 0));
		REQUIRE(approxEqual(xRot180.at(3, 2), 0));
		REQUIRE(approxEqual(xRot180.at(3, 3), 1));

		auto v2 = xRot90*v1;
		REQUIRE(approxEqual(v2[0], 1));
		REQUIRE(approxEqual(v2[1], -1));
		REQUIRE(approxEqual(v2[2], 1));
		REQUIRE(approxEqual(v2[3], 1));

		auto v3 = xRot180*v1;
		REQUIRE(approxEqual(v3[0], 1));
		REQUIRE(approxEqual(v3[1], -1));
		REQUIRE(approxEqual(v3[2], -1));
		REQUIRE(approxEqual(v3[3], 1));
	}
	SECTION("yRotationMatrix()") {
		sfz::mat4f yRot90 = sfz::yRotationMatrix(sfz::g_PI_FLOAT/2);
		REQUIRE(approxEqual(yRot90, sfz::rotationMatrix(sfz::vec3f{0,1,0}, sfz::g_PI_FLOAT/2)));

		REQUIRE(approxEqual(yRot90.at(0, 0), 0));
		REQUIRE(approxEqual(yRot90.at(0, 1), 0));
		REQUIRE(approxEqual(yRot90.at(0, 2), 1));
		REQUIRE(approxEqual(yRot90.at(0, 3), 0));

		REQUIRE(approxEqual(yRot90.at(1, 0), 0));
		REQUIRE(approxEqual(yRot90.at(1, 1), 1));
		REQUIRE(approxEqual(yRot90.at(1, 2), 0));
		REQUIRE(approxEqual(yRot90.at(1, 3), 0));

		REQUIRE(approxEqual(yRot90.at(2, 0), -1));
		REQUIRE(approxEqual(yRot90.at(2, 1), 0));
		REQUIRE(approxEqual(yRot90.at(2, 2), 0));
		REQUIRE(approxEqual(yRot90.at(2, 3), 0));

		REQUIRE(approxEqual(yRot90.at(3, 0), 0));
		REQUIRE(approxEqual(yRot90.at(3, 1), 0));
		REQUIRE(approxEqual(yRot90.at(3, 2), 0));
		REQUIRE(approxEqual(yRot90.at(3, 3), 1));

		sfz::mat4f yRot180 = sfz::yRotationMatrix(sfz::g_PI_FLOAT);
		REQUIRE(approxEqual(yRot180, sfz::rotationMatrix(sfz::vec3f{0,1,0}, sfz::g_PI_FLOAT)));

		REQUIRE(approxEqual(yRot180.at(0, 0), -1));
		REQUIRE(approxEqual(yRot180.at(0, 1), 0));
		REQUIRE(approxEqual(yRot180.at(0, 2), 0));
		REQUIRE(approxEqual(yRot180.at(0, 3), 0));

		REQUIRE(approxEqual(yRot180.at(1, 0), 0));
		REQUIRE(approxEqual(yRot180.at(1, 1), 1));
		REQUIRE(approxEqual(yRot180.at(1, 2), 0));
		REQUIRE(approxEqual(yRot180.at(1, 3), 0));

		REQUIRE(approxEqual(yRot180.at(2, 0), 0));
		REQUIRE(approxEqual(yRot180.at(2, 1), 0));
		REQUIRE(approxEqual(yRot180.at(2, 2), -1));
		REQUIRE(approxEqual(yRot180.at(2, 3), 0));

		REQUIRE(approxEqual(yRot180.at(3, 0), 0));
		REQUIRE(approxEqual(yRot180.at(3, 1), 0));
		REQUIRE(approxEqual(yRot180.at(3, 2), 0));
		REQUIRE(approxEqual(yRot180.at(3, 3), 1));

		auto v2 = yRot90*v1;
		REQUIRE(approxEqual(v2[0], 1));
		REQUIRE(approxEqual(v2[1], 1));
		REQUIRE(approxEqual(v2[2], -1));
		REQUIRE(approxEqual(v2[3], 1));

		auto v3 = yRot180*v1;
		REQUIRE(approxEqual(v3[0], -1));
		REQUIRE(approxEqual(v3[1], 1));
		REQUIRE(approxEqual(v3[2], -1));
		REQUIRE(approxEqual(v3[3], 1));
	}
	SECTION("zRotationMatrix()") {
		sfz::mat4f zRot90 = sfz::zRotationMatrix(sfz::g_PI_FLOAT/2);
		REQUIRE(approxEqual(zRot90, sfz::rotationMatrix(sfz::vec3f{0,0,1}, sfz::g_PI_FLOAT/2)));

		REQUIRE(approxEqual(zRot90.at(0, 0), 0));
		REQUIRE(approxEqual(zRot90.at(0, 1), -1));
		REQUIRE(approxEqual(zRot90.at(0, 2), 0));
		REQUIRE(approxEqual(zRot90.at(0, 3), 0));

		REQUIRE(approxEqual(zRot90.at(1, 0), 1));
		REQUIRE(approxEqual(zRot90.at(1, 1), 0));
		REQUIRE(approxEqual(zRot90.at(1, 2), 0));
		REQUIRE(approxEqual(zRot90.at(1, 3), 0));

		REQUIRE(approxEqual(zRot90.at(2, 0), 0));
		REQUIRE(approxEqual(zRot90.at(2, 1), 0));
		REQUIRE(approxEqual(zRot90.at(2, 2), 1));
		REQUIRE(approxEqual(zRot90.at(2, 3), 0));

		REQUIRE(approxEqual(zRot90.at(3, 0), 0));
		REQUIRE(approxEqual(zRot90.at(3, 1), 0));
		REQUIRE(approxEqual(zRot90.at(3, 2), 0));
		REQUIRE(approxEqual(zRot90.at(3, 3), 1));

		sfz::mat4f zRot180 = sfz::zRotationMatrix(sfz::g_PI_FLOAT);
		REQUIRE(approxEqual(zRot180, sfz::rotationMatrix(sfz::vec3f{0,0,1}, sfz::g_PI_FLOAT)));

		REQUIRE(approxEqual(zRot180.at(0, 0), -1));
		REQUIRE(approxEqual(zRot180.at(0, 1), 0));
		REQUIRE(approxEqual(zRot180.at(0, 2), 0));
		REQUIRE(approxEqual(zRot180.at(0, 3), 0));

		REQUIRE(approxEqual(zRot180.at(1, 0), 0));
		REQUIRE(approxEqual(zRot180.at(1, 1), -1));
		REQUIRE(approxEqual(zRot180.at(1, 2), 0));
		REQUIRE(approxEqual(zRot180.at(1, 3), 0));

		REQUIRE(approxEqual(zRot180.at(2, 0), 0));
		REQUIRE(approxEqual(zRot180.at(2, 1), 0));
		REQUIRE(approxEqual(zRot180.at(2, 2), 1));
		REQUIRE(approxEqual(zRot180.at(2, 3), 0));

		REQUIRE(approxEqual(zRot180.at(3, 0), 0));
		REQUIRE(approxEqual(zRot180.at(3, 1), 0));
		REQUIRE(approxEqual(zRot180.at(3, 2), 0));
		REQUIRE(approxEqual(zRot180.at(3, 3), 1));

		auto v2 = zRot90*v1;
		REQUIRE(approxEqual(v2[0], -1));
		REQUIRE(approxEqual(v2[1], 1));
		REQUIRE(approxEqual(v2[2], 1));
		REQUIRE(approxEqual(v2[3], 1));

		auto v3 = zRot180*v1;
		REQUIRE(approxEqual(v3[0], -1));
		REQUIRE(approxEqual(v3[1], -1));
		REQUIRE(approxEqual(v3[2], 1));
		REQUIRE(approxEqual(v3[3], 1));
	}
	SECTION("rotationMatrix()") {
		sfz::vec4f startPoint{1, 0, 0, 1};
		const sfz::vec4f expectedEndPoint{0, 1, 0, 1};
		sfz::vec3f axis = sfz::vec3f{1, 1, 0} - sfz::vec3f{0, 0, 0};
		sfz::mat4f rot = sfz::rotationMatrix(axis, sfz::g_PI_FLOAT);

		auto res = rot * startPoint;
		
		REQUIRE(approxEqual(res[0], expectedEndPoint[0]));
		REQUIRE(approxEqual(res[1], expectedEndPoint[1]));
		REQUIRE(approxEqual(res[2], expectedEndPoint[2]));
		REQUIRE(approxEqual(res[3], expectedEndPoint[3]));
	}
}


TEST_CASE("Transformation matrices", "[sfz::MatrixSupport]")
{
	sfz::vec4f v1{1, 1, 1, 1};
	SECTION("identityMatrix()") {
		auto m = sfz::identityMatrix<int>();
		
		REQUIRE(m.at(0, 0) == 1);
		REQUIRE(m.at(0, 1) == 0);
		REQUIRE(m.at(0, 2) == 0);
		REQUIRE(m.at(0, 3) == 0);

		REQUIRE(m.at(1, 0) == 0);
		REQUIRE(m.at(1, 1) == 1);
		REQUIRE(m.at(1, 2) == 0);
		REQUIRE(m.at(1, 3) == 0);

		REQUIRE(m.at(2, 0) == 0);
		REQUIRE(m.at(2, 1) == 0);
		REQUIRE(m.at(2, 2) == 1);
		REQUIRE(m.at(2, 3) == 0);

		REQUIRE(m.at(3, 0) == 0);
		REQUIRE(m.at(3, 1) == 0);
		REQUIRE(m.at(3, 2) == 0);
		REQUIRE(m.at(3, 3) == 1);
	}
	SECTION("scalingMatrix(scaleFactor)") {
		auto m = sfz::scalingMatrix(2.0f);

		REQUIRE(approxEqual(m.at(0, 0), 2));
		REQUIRE(approxEqual(m.at(0, 1), 0));
		REQUIRE(approxEqual(m.at(0, 2), 0));
		REQUIRE(approxEqual(m.at(0, 3), 0));

		REQUIRE(approxEqual(m.at(1, 0), 0));
		REQUIRE(approxEqual(m.at(1, 1), 2));
		REQUIRE(approxEqual(m.at(1, 2), 0));
		REQUIRE(approxEqual(m.at(1, 3), 0));

		REQUIRE(approxEqual(m.at(2, 0), 0));
		REQUIRE(approxEqual(m.at(2, 1), 0));
		REQUIRE(approxEqual(m.at(2, 2), 2));
		REQUIRE(approxEqual(m.at(2, 3), 0));

		REQUIRE(approxEqual(m.at(3, 0), 0));
		REQUIRE(approxEqual(m.at(3, 1), 0));
		REQUIRE(approxEqual(m.at(3, 2), 0));
		REQUIRE(approxEqual(m.at(3, 3), 1));

		auto v2 = m*v1;
		REQUIRE(approxEqual(v2[0], 2));
		REQUIRE(approxEqual(v2[1], 2));
		REQUIRE(approxEqual(v2[2], 2));
		REQUIRE(approxEqual(v2[3], 1));
	}
	SECTION("scalingMatrix(scaleX, scaleY, scaleZ)") {
		auto m = sfz::scalingMatrix(2.0f, 3.0f, 4.0f);

		REQUIRE(approxEqual(m.at(0, 0), 2));
		REQUIRE(approxEqual(m.at(0, 1), 0));
		REQUIRE(approxEqual(m.at(0, 2), 0));
		REQUIRE(approxEqual(m.at(0, 3), 0));

		REQUIRE(approxEqual(m.at(1, 0), 0));
		REQUIRE(approxEqual(m.at(1, 1), 3));
		REQUIRE(approxEqual(m.at(1, 2), 0));
		REQUIRE(approxEqual(m.at(1, 3), 0));

		REQUIRE(approxEqual(m.at(2, 0), 0));
		REQUIRE(approxEqual(m.at(2, 1), 0));
		REQUIRE(approxEqual(m.at(2, 2), 4));
		REQUIRE(approxEqual(m.at(2, 3), 0));

		REQUIRE(approxEqual(m.at(3, 0), 0));
		REQUIRE(approxEqual(m.at(3, 1), 0));
		REQUIRE(approxEqual(m.at(3, 2), 0));
		REQUIRE(approxEqual(m.at(3, 3), 1));

		auto v2 = m*v1;
		REQUIRE(approxEqual(v2[0], 2));
		REQUIRE(approxEqual(v2[1], 3));
		REQUIRE(approxEqual(v2[2], 4));
		REQUIRE(approxEqual(v2[3], 1));
	}
	SECTION("translationMatrix()") {
		auto m = sfz::translationMatrix(-2.0f, 1.0f, 0.0f);

		REQUIRE(approxEqual(m.at(0, 0), 1));
		REQUIRE(approxEqual(m.at(0, 1), 0));
		REQUIRE(approxEqual(m.at(0, 2), 0));
		REQUIRE(approxEqual(m.at(0, 3), -2));

		REQUIRE(approxEqual(m.at(1, 0), 0));
		REQUIRE(approxEqual(m.at(1, 1), 1));
		REQUIRE(approxEqual(m.at(1, 2), 0));
		REQUIRE(approxEqual(m.at(1, 3), 1));

		REQUIRE(approxEqual(m.at(2, 0), 0));
		REQUIRE(approxEqual(m.at(2, 1), 0));
		REQUIRE(approxEqual(m.at(2, 2), 1));
		REQUIRE(approxEqual(m.at(2, 3), 0));

		REQUIRE(approxEqual(m.at(3, 0), 0));
		REQUIRE(approxEqual(m.at(3, 1), 0));
		REQUIRE(approxEqual(m.at(3, 2), 0));
		REQUIRE(approxEqual(m.at(3, 3), 1));

		auto v2 = m*v1;
		REQUIRE(approxEqual(v2[0], -1));
		REQUIRE(approxEqual(v2[1], 2));
		REQUIRE(approxEqual(v2[2], 1));
		REQUIRE(approxEqual(v2[3], 1));
	}
}

TEST_CASE("Projection matrices", "[sfz::Matrix]")
{
	SECTION("glOrthogonalProjectionMatrix()") {
		auto m = sfz::glOrthogonalProjectionMatrix(-4.f, -2.f, 10.f, 3.f, 2.f, -50.f);

		REQUIRE(approxEqual(m.at(0, 0), 0.285714f));
		REQUIRE(approxEqual(m.at(0, 1), 0));
		REQUIRE(approxEqual(m.at(0, 2), 0));
		REQUIRE(approxEqual(m.at(0, 3), 0.142857f));

		REQUIRE(approxEqual(m.at(1, 0), 0));
		REQUIRE(approxEqual(m.at(1, 1), 0.5f));
		REQUIRE(approxEqual(m.at(1, 2), 0));
		REQUIRE(approxEqual(m.at(1, 3), 0));

		REQUIRE(approxEqual(m.at(2, 0), 0));
		REQUIRE(approxEqual(m.at(2, 1), 0));
		REQUIRE(approxEqual(m.at(2, 2), 0.0333333f));
		REQUIRE(approxEqual(m.at(2, 3), -0.666667f));

		REQUIRE(approxEqual(m.at(3, 0), 0));
		REQUIRE(approxEqual(m.at(3, 1), 0));
		REQUIRE(approxEqual(m.at(3, 2), 0));
		REQUIRE(approxEqual(m.at(3, 3), 1));		
	}
	SECTION("glPerspectiveProjectionMatrix()") {
		auto m = sfz::glPerspectiveProjectionMatrix(90.0f, 1.7778f, 0.01f, 500.0f);

		REQUIRE(approxEqual(m.at(0, 0), 0.562493f));
		REQUIRE(approxEqual(m.at(0, 1), 0));
		REQUIRE(approxEqual(m.at(0, 2), 0));
		REQUIRE(approxEqual(m.at(0, 3), 0));

		REQUIRE(approxEqual(m.at(1, 0), 0));
		REQUIRE(approxEqual(m.at(1, 1), 1));
		REQUIRE(approxEqual(m.at(1, 2), 0));
		REQUIRE(approxEqual(m.at(1, 3), 0));

		REQUIRE(approxEqual(m.at(2, 0), 0));
		REQUIRE(approxEqual(m.at(2, 1), 0));
		REQUIRE(approxEqual(m.at(2, 2), -1.00004f));
		REQUIRE(approxEqual(m.at(2, 3), -0.0200004f));

		REQUIRE(approxEqual(m.at(3, 0), 0));
		REQUIRE(approxEqual(m.at(3, 1), 0));
		REQUIRE(approxEqual(m.at(3, 2), -1));
		REQUIRE(approxEqual(m.at(3, 3), 0));	
	}
}

TEST_CASE("View matrices", "[sfz::Matrix]")
{
	SECTION("lookAt()") {
		auto m = sfz::lookAt(sfz::vec3f{1, 2, 3}, sfz::vec3f{25, 25, 25}, sfz::vec3f{0, 1, 0});

		REQUIRE(approxEqual(m.at(0, 0), -0.675725f));
		REQUIRE(approxEqual(m.at(0, 1), 0));
		REQUIRE(approxEqual(m.at(0, 2), 0.737154f));
		REQUIRE(approxEqual(m.at(0, 3), -1.53574f));

		REQUIRE(approxEqual(m.at(1, 0), -0.425328f));
		REQUIRE(approxEqual(m.at(1, 1), 0.816754f));
		REQUIRE(approxEqual(m.at(1, 2), -0.389884f));
		REQUIRE(approxEqual(m.at(1, 3), -0.0385262f));

		REQUIRE(approxEqual(m.at(2, 0), -0.602073f));
		REQUIRE(approxEqual(m.at(2, 1), -0.576987f));
		REQUIRE(approxEqual(m.at(2, 2), -0.5519f));
		REQUIRE(approxEqual(m.at(2, 3), 3.41175f));

		REQUIRE(approxEqual(m.at(3, 0), 0));
		REQUIRE(approxEqual(m.at(3, 1), 0));
		REQUIRE(approxEqual(m.at(3, 2), 0));
		REQUIRE(approxEqual(m.at(3, 3), 1));		
	}
}
