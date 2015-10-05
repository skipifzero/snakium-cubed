#pragma once
#ifndef S3_GAMELOGIC_DIRECTION_HPP
#define S3_GAMELOGIC_DIRECTION_HPP

#include <cstdint> // uint8_t

#include <sfz/math/Vector.hpp>

namespace s3 {

using sfz::vec3;
using std::uint8_t;

// Direction enum
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/**
 * @brief Enum representing a 3-dimensional directon.
 * The coordinate system used is a right-handed system with positive y being up (and therefore
 * positive z being towards the camera). FORWARD represents the direction the camera is looking
 * in (negative z), UP, DOWN, LEFT and RIGHT represents the directions a camera looking at
 * FORWARD can diverge of towards.
 */
enum class Direction : uint8_t {
	BACKWARD = 0, // z+
	FORWARD = 1, // z-
	UP = 2, // y+
	DOWN = 3, // y-
	RIGHT = 4, // x+
	LEFT = 5 // x-
};

// Direction functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Direction opposite(Direction direction) noexcept;
vec3 toVector(Direction direction) noexcept;
const char* to_string(Direction direction) noexcept;

/**
 * @param side the backward direction into the camera if looking at the cube's side
 * @param up the the up direction of the camera
 */

Direction left(Direction side, Direction up) noexcept;
Direction right(Direction side, Direction up) noexcept;
bool isLeftTurn(Direction side, Direction from, Direction to) noexcept;
bool isRightTurn(Direction side, Direction from, Direction to) noexcept;

Direction defaultUp(Direction side) noexcept;


// 3D Direction enum
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/**
 * @brief Enum representing a 3-dimensional directon.
 * For example used to determine which side of a cube a tile is on, NORTH == BACK, SOUTH == FRONT,
 * WEST == LEFT, EAST == RIGHT, UP == TOP, DOWN == BOTTOM.
 * It's also assumed that the coordinate system is a right-handed system with positive y being up.
 * I.e. SOUTH is in positive z-axis, UP is in positive y-axis and EAST is in positive x-axis.
 */
/*enum class Direction3D : uint8_t {
	NORTH = 0,
	SOUTH = 1,
	WEST = 2,
	EAST = 3,
	UP = 4,
	DOWN = 5
};

Direction3D up(Direction3D side, Direction3D sideRelativeUp) noexcept;
Direction3D down(Direction3D side, Direction3D sideRelativeUp) noexcept;
Direction3D left(Direction3D side, Direction3D sideRelativeUp) noexcept;
Direction3D right(Direction3D side, Direction3D sideRelativeUp) noexcept;
Direction3D defaultUp(Direction3D side) noexcept;
Direction3D map(Direction3D side, Direction3D sideRelativeUp, Direction2D dir) noexcept;
Direction3D mapDefaultUp(Direction3D side, Direction2D dir) noexcept;
Direction2D unMap(Direction3D side, Direction3D sideRelativeUp, Direction3D dir) noexcept;
Direction2D unMapDefaultUp(Direction3D side, Direction3D dir) noexcept;
vec3 toVector(Direction3D direction) noexcept;
const char* to_string(const Direction3D& direction) noexcept;
std::ostream& operator<< (std::ostream& ostream, const Direction3D& direction) noexcept;*/

} // namespace s3
#endif