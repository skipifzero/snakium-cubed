#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <vector>

#include "sfz/Geometry.hpp"
#include "sfz/Math.hpp"

TEST_CASE("Signed distance to plane", "[sfz::Plane]")
{
	using namespace sfz;

	Plane p{vec3{0.0f, 1.0f, 0.0f}, vec3{2.0f, 1.0f, 0.0f}};

	REQUIRE(approxEqual<float>(p.signedDistance(vec3{2.0f, 3.0f, 0.0f}), 2.0f));
	REQUIRE(approxEqual<float>(p.signedDistance(vec3{0.0f, 3.0f, 0.0f}), 2.0f));
	REQUIRE(approxEqual<float>(p.signedDistance(vec3{2.0f, 0.0f, 0.0f}), -1.0f));
}

TEST_CASE("Point inside AABB test", "[sfz::Intersection]")
{
	using namespace sfz;

	AABB box{vec3{-1,-1,-1}, vec3{1,1,1}};

	REQUIRE(pointInside(box, vec3{0,0,0}));
	REQUIRE(!pointInside(box, vec3{-2,0,0}));
}

TEST_CASE("AABB vs AABB test", "[sfz::Intersection]")
{
	using namespace sfz;

	AABB boxMid{vec3{-1.0f, -1.0f, -1.0f}, vec3{1.0f, 1.0f, 1.0f}};
	AABB boxMidSmall{vec3{-0.5f, -0.5f, -0.5f}, vec3{0.5f, 0.5f, 0.5f}};

	AABB boxFrontSmall{vec3{-0.5f, -0.5f, 0.75f}, vec3{0.5f, 0.5f, 1.75f}};
	AABB boxBackSmall{vec3{-0.5f, -0.5f, -1.75f}, vec3{0.5f, 0.5f, -0.75f}};

	AABB boxUpSmall{vec3{-0.5f, 0.75f, -0.5f}, vec3{0.5f, 1.75f, 0.5f}};
	AABB boxDownSmall{vec3{-0.5f, -1.75f, -0.5f}, vec3{0.5f, -0.75f, 0.5f}};

	AABB boxLeftSmall{vec3{-1.75f, -0.5f, -0.5f}, vec3{-0.75f, 0.5f, 0.5f}};
	AABB boxRightSMall{vec3{0.75f, -0.5f, -0.5f}, vec3{1.75f, 0.5f, 0.5f}};

	std::vector<AABB*> smallSurroundingBoxes;
	smallSurroundingBoxes.push_back(&boxFrontSmall);
	smallSurroundingBoxes.push_back(&boxBackSmall);
	smallSurroundingBoxes.push_back(&boxUpSmall);
	smallSurroundingBoxes.push_back(&boxDownSmall);
	smallSurroundingBoxes.push_back(&boxLeftSmall);
	smallSurroundingBoxes.push_back(&boxRightSMall);

	REQUIRE(intersects(boxMidSmall, boxMid));

	for (AABB* boxPtr : smallSurroundingBoxes) {
		REQUIRE(intersects(boxMid, *boxPtr));
	}

	for (AABB* boxPtr : smallSurroundingBoxes) {
		REQUIRE(!intersects(boxMidSmall, *boxPtr));
	}

	for (AABB* boxPtr1 : smallSurroundingBoxes) {
		for (AABB* boxPtr2 : smallSurroundingBoxes) {
			if (boxPtr1 == boxPtr2) REQUIRE(intersects(*boxPtr1, *boxPtr2));
			else REQUIRE(!intersects(*boxPtr1, *boxPtr2));
		}
	}
}

TEST_CASE("OBB vs OBB test", "[sfz::Intersection]")
{
	using namespace sfz;

	std::array<vec3,3> axisAlignedAxes{{vec3{1.0f, 0.0f, 0.0f},
	                                    vec3{0.0f, 1.0f, 0.0f},
	                                    vec3{0.0f, 0.0f, 1.0f}}};
	vec3 smallExts{1.0f, 1.0f, 1.0f};
	vec3 bigExts{2.0f, 2.0f, 2.0f};

	OBB midSmallAA{vec3{0.0f, 0.0f, 0.0f}, axisAlignedAxes, smallExts};
	OBB midSmallLeftAA{vec3{-1.0f, 0.0f, 0.0f}, axisAlignedAxes, smallExts};
	OBB midSmallRightAA{vec3{1.0f, 0.0f, 0.0f}, axisAlignedAxes, smallExts};
	OBB midSmallDownAA{vec3{0.0f, -1.0f, 0.0f}, axisAlignedAxes, smallExts};
	OBB midSmallUpAA{vec3{0.0f, 1.0f, 0.0f}, axisAlignedAxes, smallExts};
	OBB midSmallBackAA{vec3{0.0f, 0.0f, -1.0f}, axisAlignedAxes, smallExts};
	OBB midSmallFrontAA{vec3{0.0f, 0.0f, 1.0f}, axisAlignedAxes, smallExts};
	std::vector<OBB*> smallSurroundingAABoxes;
	smallSurroundingAABoxes.push_back(&midSmallLeftAA);
	smallSurroundingAABoxes.push_back(&midSmallRightAA);
	smallSurroundingAABoxes.push_back(&midSmallDownAA);
	smallSurroundingAABoxes.push_back(&midSmallUpAA);
	smallSurroundingAABoxes.push_back(&midSmallBackAA);
	smallSurroundingAABoxes.push_back(&midSmallFrontAA);

	OBB midAA{vec3{0.0f, 0.0f, 0.0f}, axisAlignedAxes, bigExts};
	OBB midLeftAA{vec3{-1.0f, 0.0f, 0.0f}, axisAlignedAxes, bigExts};
	OBB midRightAA{vec3{1.0f, 0.0f, 0.0f}, axisAlignedAxes, bigExts};
	OBB midDownAA{vec3{0.0f, -1.0f, 0.0f}, axisAlignedAxes, bigExts};
	OBB midUpAA{vec3{0.0f, 1.0f, 0.0f}, axisAlignedAxes, bigExts};
	OBB midBackAA{vec3{0.0f, 0.0f, -1.0f}, axisAlignedAxes, bigExts};
	OBB midFrontAA{vec3{0.0f, 0.0f, 1.0f}, axisAlignedAxes, bigExts};
	std::vector<OBB*> surroundingAABoxes;
	surroundingAABoxes.push_back(&midLeftAA);
	surroundingAABoxes.push_back(&midRightAA);
	surroundingAABoxes.push_back(&midDownAA);
	surroundingAABoxes.push_back(&midUpAA);
	surroundingAABoxes.push_back(&midBackAA);
	surroundingAABoxes.push_back(&midFrontAA);

	for (OBB* smallPtr : smallSurroundingAABoxes) {
		REQUIRE(intersects(*smallPtr, midAA));
		REQUIRE(intersects(midAA, *smallPtr));
	}

	REQUIRE(!intersects(midSmallLeftAA, midSmallRightAA));
	REQUIRE(!intersects(midSmallDownAA, midSmallUpAA));
	REQUIRE(!intersects(midSmallBackAA, midSmallFrontAA));

	REQUIRE(!intersects(midSmallLeftAA, midRightAA));
	REQUIRE(!intersects(midSmallDownAA, midUpAA));
	REQUIRE(!intersects(midSmallBackAA, midFrontAA));

	REQUIRE(!intersects(midLeftAA, midSmallRightAA));
	REQUIRE(!intersects(midDownAA, midSmallUpAA));
	REQUIRE(!intersects(midBackAA, midSmallFrontAA));
}

TEST_CASE("Sphere vs Sphere test", "[sfz::Intersection]")
{
	using namespace sfz;

	Sphere mid{vec3{0.0f, 0.0f, 0.0f}, 0.5f};
	Sphere midBig{vec3{0.0f, 0.0f, 0.0f}, 1.0f};
	Sphere aBitOff{vec3{-1.1f, 0.0f, 0.0f}, 0.5f};

	REQUIRE(intersects(mid, midBig));
	REQUIRE(intersects(midBig, aBitOff));
	REQUIRE(!intersects(mid, aBitOff));
}

TEST_CASE("Plane vs AABB test", "[sfz::Intersection]")
{
	using namespace sfz;

	Plane p1{vec3{0.0f, 1.0f, 0.0f}, vec3{0.0f, 0.5f, 0.0f}};
	Plane p2{vec3{0.0f, 1.0f, 0.0f}, vec3{0.0f, 1.5f, 0.0f}};
	AABB aabb{vec3{1.0f, 1.0f, 1.0f}, vec3{3.0f, 3.0f, 3.0f}};

	REQUIRE(!intersects(p1, aabb));
	REQUIRE(intersects(p2, aabb));
}

TEST_CASE("Plane vs OBB test", "[sfz::Intersection]")
{
	using namespace sfz;

	Plane p1{vec3{0.0f, 1.0f, 0.0f}, vec3{0.0f, 0.5f, 0.0f}};
	Plane p2{vec3{0.0f, 1.0f, 0.0f}, vec3{0.0f, 1.5f, 0.0f}};
	OBB obb{AABB{vec3{1.0f, 1.0f, 1.0f}, vec3{3.0f, 3.0f, 3.0f}}};

	REQUIRE(!intersects(p1, obb));
	REQUIRE(intersects(p2, obb));
}