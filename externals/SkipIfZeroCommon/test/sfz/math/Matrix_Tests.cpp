#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "sfz/math/Vector.hpp"
#include "sfz/math/Matrix.hpp"

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
	REQUIRE(std::is_pod<sfz::mat4f>::value);
}