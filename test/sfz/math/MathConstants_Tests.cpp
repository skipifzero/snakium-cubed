#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "sfz/math/MathConstants.hpp"

TEST_CASE("Pi constants", "[MathConstants]")
{
	REQUIRE(3.1415f <= sfz::PI());
	REQUIRE(sfz::PI() <= 3.1416f);
	REQUIRE(sfz::PI<int>() == 3);
}