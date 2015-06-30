#pragma once
#ifndef S3_GAMELOGIC_DIRECTION_HPP
#define S3_GAMELOGIC_DIRECTION_HPP

#include <cstdint> // uint8_t
#include <iostream>
#include <exception> // std::terminate
#include <string>
#include <iostream> // std::ostream
#include <sfz/math/Vector.hpp>

namespace s3 {

using std::uint8_t;

// 2D Direction enum
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/**
 * @brief Enum representing a direction on a SnakeTile.
 * Needs 2 bits.
 */
enum class Direction2D : uint8_t {
	UP = 0,
	DOWN = 1,
	LEFT = 2,
	RIGHT = 3
};

inline Direction2D opposite(Direction2D direction) noexcept;
inline Direction2D toLeft(Direction2D from) noexcept;
inline Direction2D toRight(Direction2D from) noexcept;
inline bool isTurn(Direction2D from, Direction2D to) noexcept;
inline bool isLeftTurn(Direction2D from, Direction2D to) noexcept;
inline bool isRightTurn(Direction2D form, Direction2D to) noexcept;
inline std::string to_string(const Direction2D& direction) noexcept;
inline std::ostream& operator<< (std::ostream& ostream, const Direction2D& direction) noexcept;

// 3D Direction enum
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/**
 * @brief Enum representing which side of a cube a tile is one
 * It's assumed that the coordinate system is a right-handed system with positive y being up. I.e.
 * FRONT is in positive z-axis and TOP is in positive y-axis.
 */

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

inline Direction3D opposite(Direction3D direction) noexcept;
inline Direction3D up(Direction3D side, Direction3D sideRelativeUp) noexcept;
inline Direction3D down(Direction3D side, Direction3D sideRelativeUp) noexcept;
inline Direction3D left(Direction3D side, Direction3D sideRelativeUp) noexcept;
inline Direction3D right(Direction3D side, Direction3D sideRelativeUp) noexcept;
inline Direction3D defaultUp(Direction3D side) noexcept;
inline Direction3D map(Direction3D side, Direction3D sideRelativeUp, Direction2D dir) noexcept;
inline Direction3D mapDefaultUp(Direction3D side, Direction2D dir) noexcept;
inline Direction2D unMap(Direction3D side, Direction3D sideRelativeUp, Direction3D dir) noexcept;
inline Direction2D unMapDefaultUp(Direction3D side, Direction3D dir) noexcept;
inline sfz::vec3f toVector(Direction3D direction) noexcept;
inline std::string to_string(const Direction3D& direction) noexcept;
inline std::ostream& operator<< (std::ostream& ostream, const Direction3D& direction) noexcept;

} // namespace s3
#include "gamelogic/Direction.inl"
#endif