#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "sfz/math/Vector.hpp"
#include "sfz/math/VectorSupport.hpp"

#include <unordered_map>
#include <type_traits>

TEST_CASE("Constructors", "[sfz::Vector]")
{
	SECTION("Initalizer list constructor assigns correct value to elements") {
		sfz::Vector<int, 4> vector = {-2, 2, 1, 42};
		REQUIRE(vector[0] == -2);
		REQUIRE(vector[1] == 2);
		REQUIRE(vector[2] == 1);
		REQUIRE(vector[3] == 42);

		sfz::Vector<int, 4> v2 = {-2, 2, 1};
		REQUIRE(v2[0] == -2);
		REQUIRE(v2[1] == 2);
		REQUIRE(v2[2] == 1);
		REQUIRE(v2[3] == 0);
	}
	SECTION("Copy constructor correctly copies vector") {
		sfz::Vector<int, 4> vector{sfz::Vector<int, 4>{-2, 2, 1, 42}};
		REQUIRE(vector[0] == -2);
		REQUIRE(vector[1] == 2);
		REQUIRE(vector[2] == 1);
		REQUIRE(vector[3] == 42);
	}
}

TEST_CASE("Assignment and accessing", "[sfz::Vector]")
{
	sfz::Vector<int, 5> vector{-10, 10, 12, 13, -2};
	SECTION("Correct start values") {
		REQUIRE(vector[0] == -10);
		REQUIRE(vector.at(1) == 10);
		REQUIRE(vector[2] == 12);
		REQUIRE(vector[3] == 13);
		REQUIRE(vector[4] == -2);
	}
	SECTION("Assignment with set() function") {
		vector.set(0, 2);
		REQUIRE(vector.at(0) == 2);
		vector.set(3, -100);
		REQUIRE(vector.at(3) == -100);
	}
	SECTION("Assignment with [] operator") {
		vector[1] = 4242;
		REQUIRE(vector[1] == 4242);
		vector[4] = 54;
		REQUIRE(vector[4] == 54);
	}
}

TEST_CASE("glPtr()", "[sfz::Vector]") {
	sfz::Vector<int, 2> v1{1, 2};
	const sfz::Vector<int, 2> v2{-1, -2};
	
	int* ptr1 = v1.glPtr();
	const int* ptr2 = v2.glPtr();

	REQUIRE(ptr1[0] == 1);
	REQUIRE(ptr1[1] == 2);

	REQUIRE(ptr2[0] == -1);
	REQUIRE(ptr2[1] == -2);
}

TEST_CASE("fill", "[sfz::Vector]")
{
	sfz::Vector<int,4> v;
	v.fill(4);
	for (auto e : v) {
		REQUIRE(e == 4);
	}
	v.fill(2);
	for (auto e : v) {
		REQUIRE(e == 2);
	}
}

TEST_CASE("Arithmetic operators", "[sfz::Vector]")
{
	sfz::Vector<int, 3> v1{1, -2, 5};
	sfz::Vector<int, 3> v2{0, -2, 1};

	// Integrity check of base vectors
	REQUIRE(v1[0] == 1);
	REQUIRE(v1[1] == -2);
	REQUIRE(v1[2] == 5);
	REQUIRE(v2[0] == 0);
	REQUIRE(v2[1] == -2);
	REQUIRE(v2[2] == 1);

	SECTION("Addition") {
		auto v3 = v1 + v2;
		REQUIRE(v3[0] == 1);
		REQUIRE(v3[1] == -4);
		REQUIRE(v3[2] == 6);
		// Integrity check of base vectors
		REQUIRE(v1[0] == 1);
		REQUIRE(v1[1] == -2);
		REQUIRE(v1[2] == 5);
		REQUIRE(v2[0] == 0);
		REQUIRE(v2[1] == -2);
		REQUIRE(v2[2] == 1);
	}
	SECTION("Subtraction") {
		auto v3 = v1 - v2;
		REQUIRE(v3[0] == 1);
		REQUIRE(v3[1] == 0);
		REQUIRE(v3[2] == 4);
		auto v4 = v2 - v1;
		REQUIRE(v4[0] == -1);
		REQUIRE(v4[1] == 0);
		REQUIRE(v4[2] == -4);
		// Integrity check of base vectors
		REQUIRE(v1[0] == 1);
		REQUIRE(v1[1] == -2);
		REQUIRE(v1[2] == 5);
		REQUIRE(v2[0] == 0);
		REQUIRE(v2[1] == -2);
		REQUIRE(v2[2] == 1);
	}
	SECTION("Negating (-x)") {
		auto v3 = -v1;
		REQUIRE(v3[0] == -1);
		REQUIRE(v3[1] == 2);
		REQUIRE(v3[2] == -5);
		// Integrity check of base vectors
		REQUIRE(v1[0] == 1);
		REQUIRE(v1[1] == -2);
		REQUIRE(v1[2] == 5);
		REQUIRE(v2[0] == 0);
		REQUIRE(v2[1] == -2);
		REQUIRE(v2[2] == 1);
	}
	SECTION("Multiplication by number") {
		auto v3 = v1*3;
		REQUIRE(v3[0] == 3);
		REQUIRE(v3[1] == -6);
		REQUIRE(v3[2] == 15);
		auto v4 = -3*v2;
		REQUIRE(v4[0] == 0);
		REQUIRE(v4[1] == 6);
		REQUIRE(v4[2] == -3);
		// Integrity check of base vectors
		REQUIRE(v1[0] == 1);
		REQUIRE(v1[1] == -2);
		REQUIRE(v1[2] == 5);
		REQUIRE(v2[0] == 0);
		REQUIRE(v2[1] == -2);
		REQUIRE(v2[2] == 1);
	}
	SECTION("Division by number") {
		auto v1 = sfz::Vector<int, 2>{2, -2}/2;
		REQUIRE(v1[0] == 1);
		REQUIRE(v1[1] == -1);
	}
	SECTION("Addition assignment") {
		v1 += v2;
		REQUIRE(v1[0] == 1);
		REQUIRE(v1[1] == -4);
		REQUIRE(v1[2] == 6);
	}
	SECTION("Subtraction assignment") {
		v1 -= v2;
		REQUIRE(v1[0] == 1);
		REQUIRE(v1[1] == 0);
		REQUIRE(v1[2] == 4);
	}
	SECTION("Multiplication by number assignment") {
		v1 *= 3;
		REQUIRE(v1[0] == 3);
		REQUIRE(v1[1] == -6);
		REQUIRE(v1[2] == 15);
	}
	SECTION("Division by number assignment") {
		sfz::Vector<int, 2> v3{2, -2};
		v3 /= 2;
		REQUIRE(v3[0] == 1);
		REQUIRE(v3[1] == -1);
	}
}

TEST_CASE("Norm (length) of vector", "[sfz::Vector]")
{
	sfz::Vector<int, 2> v1{2, 0};
	sfz::Vector<int, 5> v2{-2, 2, 2, -2, 3};

	SECTION("squaredNorm()") {
		REQUIRE(v1.squaredNorm() == 4);
		REQUIRE(v2.squaredNorm() == 25);
	}
	SECTION("v.norm()") {
		REQUIRE(v1.norm() == 2);
		REQUIRE(v2.norm() == 5);
	}
	SECTION("Rounding down") {
		sfz::Vector<int, 2> v3{2,1};
		REQUIRE(v3.squaredNorm() == 5);
		REQUIRE(v3.norm() == 2);
	}
}

TEST_CASE("Normalizing (making unit vector) vector", "[sfz::Vector]")
{
	sfz::Vector<float, 4> v1 = sfz::Vector<float, 4>{-2.f, 2.f, -2.f, 2.f}.normalize();
	const float delta = 1e-3f;

	SECTION("Correct answer") {
		const float posLower = 0.5f - delta;
		const float posHigher = 0.5f + delta;
		const float negLower = -0.5f - delta;
		const float negHigher = -0.5f + delta;		

		REQUIRE(negLower <= v1[0]);
		REQUIRE(v1[0] <= negHigher);

		REQUIRE(posLower <= v1[1]);
		REQUIRE(v1[1] <= posHigher);

		REQUIRE(negLower <= v1[2]);
		REQUIRE(v1[2] <= negHigher);

		REQUIRE(posLower <= v1[3]);
		REQUIRE(v1[3] <= posHigher);
	}
}

TEST_CASE("Comparison operators", "[sfz::Vector]")
{
	sfz::Vector<int, 3> v1{-4, 0, 0};
	sfz::Vector<int, 3> v2{0, 2, 0};
	sfz::Vector<int, 3> v3{0, 2, 0};

	SECTION("== and !=") {
		REQUIRE(v1 == v1);
		REQUIRE(v2 == v2);
		REQUIRE(v3 == v3);	
		REQUIRE(v2 == v3);
		REQUIRE(v3 == v2);
		REQUIRE(v1 != v2);
		REQUIRE(v2 != v1);
	}
	SECTION("< and >") {
		REQUIRE(v2 < v1);
		REQUIRE(!(v2 > v1));
		REQUIRE(v1 > v2);
		REQUIRE(!(v1 < v2));
	}
	SECTION("<= and >=") {
		REQUIRE(v2 <= v1);
		REQUIRE(!(v2 >= v1));
		REQUIRE(v1 >= v2);
		REQUIRE(!(v1 <= v2));
		REQUIRE(v2 <= v3);
		REQUIRE(v2 >= v3);
		REQUIRE(v3 <= v2);
		REQUIRE(v3 >= v2);
	}
}

TEST_CASE("Dot (scalar) product", "[sfz::Vector]")
{
	SECTION("Correctness test") {
		sfz::Vector<int, 3> v1{1, 0, -2};
		sfz::Vector<int, 3> v2{6, 2, 2};
		int scalarProduct = v1.dot(v2);
		
		REQUIRE(scalarProduct == 2);
		
		REQUIRE(v1[0] == 1);
		REQUIRE(v1[1] == 0);
		REQUIRE(v1[2] == -2);
		REQUIRE(v2[0] == 6);
		REQUIRE(v2[1] == 2);
		REQUIRE(v2[2] == 2);
	}
	SECTION("Using same vector twice") {	
		sfz::Vector<int, 2> v1{-3, 2};
		int scalarProduct = v1.dot(v1);
		
		REQUIRE(scalarProduct == 13);
		
		REQUIRE(v1[0] == -3);
		REQUIRE(v1[1] == 2);	
	}
}

TEST_CASE("Element-wise multiplication", "[sfz::Vector]")
{
	sfz::Vector<int, 3> v1{1, 0, -3};
	sfz::Vector<int, 3> v2{2, 0, 2};

	SECTION("Correctness test") {
		auto v3 = v1.elemMult(v2);
		REQUIRE(v3[0] == 2);
		REQUIRE(v3[1] == 0);
		REQUIRE(v3[2] == -6);
	}
	SECTION("Same vector") {
		auto v3 = v1.elemMult(v1);
		REQUIRE(v3[0] == 1);
		REQUIRE(v3[1] == 0);
		REQUIRE(v3[2] == 9);
	}
}

TEST_CASE("Sum of vector", "[sfz::Vector]")
{
	sfz::Vector<int, 4> v1{1, 2, -4, 9};
	REQUIRE(v1.sum() == 8);
}

TEST_CASE("Converting to string", "[sfz::Vector]")
{
	sfz::Vector<int, 3> v{-1, 2, 10};
	REQUIRE(v.to_string() == "[-1, 2, 10]");
}

TEST_CASE("Hashing", "[sfz::Vector]")
{
	sfz::Vector<int, 3> v1{2, 100, 32};
	sfz::Vector<int, 3> v2{-1, 0, -10};
	sfz::Vector<int, 3> v3{0, -9, 14};

	// This test checks if unordered_map works as it should. Not a very good test, but the best I
	// can come up with to test if hashing works as it should at the moment.
	std::unordered_map<sfz::Vector<int, 3>, int> hashMap;
	hashMap[v1] = 1;
	hashMap[v2] = 2;
	hashMap[v3] = 3;
	REQUIRE(hashMap[v1] == 1);
	REQUIRE(hashMap[v2] == 2);
	REQUIRE(hashMap[v3] == 3);

	REQUIRE(v1.hash() != v2.hash());
	REQUIRE(v2.hash() != v3.hash());
}

TEST_CASE("Is proper POD", "[sfz::Vector]")
{
	REQUIRE(std::is_trivially_default_constructible<sfz::vec2f>::value);
	REQUIRE(std::is_trivially_default_constructible<sfz::vec2i>::value);
	REQUIRE(std::is_trivially_default_constructible<sfz::vec3f>::value);
	REQUIRE(std::is_trivially_default_constructible<sfz::vec3i>::value);

	REQUIRE(std::is_trivially_copyable<sfz::vec2f>::value);
	REQUIRE(std::is_trivially_copyable<sfz::vec2i>::value);
	REQUIRE(std::is_trivially_copyable<sfz::vec3f>::value);
	REQUIRE(std::is_trivially_copyable<sfz::vec3i>::value);

	REQUIRE(std::is_trivial<sfz::vec2f>::value);
	REQUIRE(std::is_trivial<sfz::vec2i>::value);
	REQUIRE(std::is_trivial<sfz::vec3f>::value);
	REQUIRE(std::is_trivial<sfz::vec3i>::value);

	REQUIRE(std::is_standard_layout<sfz::vec2f>::value);
	REQUIRE(std::is_standard_layout<sfz::vec2i>::value);
	REQUIRE(std::is_standard_layout<sfz::vec3f>::value);
	REQUIRE(std::is_standard_layout<sfz::vec3i>::value);

#ifndef _MSC_VER
	REQUIRE(std::is_pod<sfz::vec2f>::value);
	REQUIRE(std::is_pod<sfz::vec2i>::value);
	REQUIRE(std::is_pod<sfz::vec3f>::value);
	REQUIRE(std::is_pod<sfz::vec3i>::value);
#endif

	REQUIRE(std::is_literal_type<sfz::vec2f>::value);
	REQUIRE(std::is_literal_type<sfz::vec2i>::value);
	REQUIRE(std::is_literal_type<sfz::vec3f>::value);
	REQUIRE(std::is_literal_type<sfz::vec3i>::value);
}


// VectorSupport.hpp
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


TEST_CASE("Cross product", "[sfz::VectorSupport]")
{
	sfz::Vector<int, 3> v1{-1, 4, 0};
	sfz::Vector<int, 3> v2{1, -2, 3};

	SECTION("Correctness test") {
		auto res = sfz::cross(v1, v2);

		REQUIRE(res[0] == 12);
		REQUIRE(res[1] == 3);
		REQUIRE(res[2] == -2);
	}
	SECTION("2nd correctness test") {
		auto res = sfz::cross(v2, v1);

		REQUIRE(res[0] == -12);
		REQUIRE(res[1] == -3);
		REQUIRE(res[2] == 2);
	}
	SECTION("A x A = 0") {
		auto res1 = sfz::cross(v1, v1);
		REQUIRE(res1[0] == 0);
		REQUIRE(res1[1] == 0);
		REQUIRE(res1[2] == 0);

		auto res2 = sfz::cross(v2, v2);
		REQUIRE(res2[0] == 0);
		REQUIRE(res2[1] == 0);
		REQUIRE(res2[2] == 0);
	}
}

TEST_CASE("Angle of vectors", "[sfz::VectorSupport]")
{
	sfz::Vector<float, 2> vRight{1, 0};
	sfz::Vector<float, 2> vUp{0, 1};
	sfz::Vector<float, 2> vDown{0, -1};

	SECTION("(2D) Angle between y and (implicit) x-axis") {
		auto angle = sfz::angle(vUp);
		REQUIRE((3.1415f/2.f) <= angle);
		REQUIRE(angle <= (3.1416f/2.f));
	}
	SECTION("Angle between y and (explicit) x-axis") {
		auto angle = sfz::angle(vRight, vUp);
		REQUIRE((3.1415f/2.f) <= angle);
		REQUIRE(angle <= (3.1416f/2.f));
	}
	SECTION("Angle between same vectors") {
		auto angle1 = angle(vRight);
		REQUIRE(angle1 == 0);

		auto angle2 = angle(vUp, vUp);
		REQUIRE(angle2 == 0);
	}
	SECTION("(2D) Angle with implicit x-axis should never give negative angles") {
		auto angle = sfz::angle(vDown);
		REQUIRE((3.f*3.1415f/2.f) <= angle);
		REQUIRE(angle <= (3.f*3.1416f/2.f));
	}
}

TEST_CASE("Rotating vectors", "[sfz::VectorSupport]")
{
	sfz::Vector<float, 2> vRight{1, 0};
	sfz::Vector<float, 2> vUp{0, 1};
	
	SECTION("Rotates in positive direction") {
		auto res = sfz::rotate(vRight, 3.1415926f);
		REQUIRE(-1.01f <= res[0]);
		REQUIRE(res[0] <= -0.99f);
		REQUIRE(-0.01f <= res[1]);
		REQUIRE(res[1] <= 0.01f);

		auto angleOrg = angle(vRight);
		auto angleRes = angle(res);
		REQUIRE((angleOrg + 3.1415f) <= angleRes);
		REQUIRE(angleRes <= (angleOrg + 3.1416f));
	}
	SECTION("Rotates in negative direction") {
		auto res = sfz::rotate(vUp, -3.1415926f);
		REQUIRE(-0.01f <= res[0]);
		REQUIRE(res[0] <= 0.01f);
		REQUIRE(-1.01f <= res[1]);
		REQUIRE(res[1] <= -0.99f);

		auto angleOrg = angle(vUp);
		auto angleRes = angle(res);
		// "+" in this case since angle() returns positive angle from x-axis.
		REQUIRE((angleOrg + 3.1415f) <= angleRes);
		REQUIRE(angleRes <= (angleOrg + 3.1416f));
	}
	SECTION("Nothing happens when rotating with 0") {
		auto resRight = sfz::rotate(vRight, 0.f);
		REQUIRE(resRight == vRight);
		auto resUp = sfz::rotate(vUp, 0.f);
		REQUIRE(resUp == vUp);
	}
}

TEST_CASE("Projecting a vector onto another vector", "[sfz::VectorSupport]")
{
	sfz::Vector<int, 2> vUp{0, 2};
	sfz::Vector<int, 2> vRight{3, 0};
	sfz::Vector<int, 2> v{9, 12};

	SECTION("Basic test") {
		auto res = projectOnto(v, vUp);
		REQUIRE(res[0] == 0);
		REQUIRE(res[1] == 12);
	}
	SECTION("Basic test 2") {
		auto res = projectOnto(v, vRight);
		REQUIRE(res[0] == 9);
		REQUIRE(res[1] == 0);
	}
	SECTION("Returns 0 vector if target is 0 vector.") {
		auto zero = sfz::Vector<int,2>{0,0};
		auto res = projectOnto(v, zero);
		REQUIRE(res == zero);
	}
}