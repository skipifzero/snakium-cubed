#pragma once
#ifndef S3_GAMELOGIC_DIRECTION_HPP
#define S3_GAMELOGIC_DIRECTION_HPP

#include <cstdint> // uint8_t
#include <iostream> // std::ostream

#include <sfz/math/Vector.hpp>

namespace s3 {

using sfz::vec3;
using std::uint8_t;

// 2D Direction enum
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/** Enum representing a direction on a SnakeTile. */
enum class Direction2D : uint8_t {
	UP = 0,
	DOWN = 1,
	LEFT = 2,
	RIGHT = 3
};

Direction2D opposite(Direction2D direction) noexcept;
Direction2D toLeft(Direction2D from) noexcept;
Direction2D toRight(Direction2D from) noexcept;
bool isTurn(Direction2D from, Direction2D to) noexcept;
bool isLeftTurn(Direction2D from, Direction2D to) noexcept;
bool isRightTurn(Direction2D form, Direction2D to) noexcept;
const char* to_string(const Direction2D& direction) noexcept;
std::ostream& operator<< (std::ostream& ostream, const Direction2D& direction) noexcept;

// 3D Direction enum
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/**
 * @brief Enum representing a 3-dimensional directon.
 * For example used to determine which side of a cube a tile is on, NORTH == BACK, SOUTH == FRONT,
 * WEST == LEFT, EAST == RIGHT, UP == TOP, DOWN == BOTTOM.
 * It's also assumed that the coordinate system is a right-handed system with positive y being up.
 * I.e. SOUTH is in positive z-axis, UP is in positive y-axis and EAST is in positive x-axis.
 */
enum class Direction3D : uint8_t {
	NORTH = 0,
	SOUTH = 1,
	WEST = 2,
	EAST = 3,
	UP = 4,
	DOWN = 5
};

Direction3D opposite(Direction3D direction) noexcept;
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
std::ostream& operator<< (std::ostream& ostream, const Direction3D& direction) noexcept;

} // namespace s3
#endif