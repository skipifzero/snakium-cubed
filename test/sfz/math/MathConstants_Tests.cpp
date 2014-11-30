#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "sfz/math/MathConstants.hpp"

TEST_CASE("Pi constants", "[MathConstants]")
{
	SECTION("Float version") {
		REQUIRE(3.1415f <= sfz::g_PI_FLOAT);
		REQUIRE(sfz::g_PI_FLOAT <= 3.1416f);
	}
	SECTION("Double version") {
		REQUIRE(3.1415 <= sfz::g_PI_DOUBLE);
		REQUIRE(sfz::g_PI_DOUBLE <= 3.1416);
	}
}