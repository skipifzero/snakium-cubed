#pragma once
#ifndef S3_GAMELOGIC_DIRECTION_HPP
#define S3_GAMELOGIC_DIRECTION_HPP

#include <cstdint> // uint8_t

#include <sfz/math/Vector.hpp>

namespace s3 {

using sfz::vec3;
using sfz::vec3i;
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

/**
 * @brief Simplified direction enum used for input to model
 */
enum class DirectionInput : uint8_t {
	UP, DOWN, LEFT, RIGHT, SHIFT
};

// Direction functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Direction opposite(Direction direction) noexcept;
DirectionInput opposite(DirectionInput direction) noexcept;
vec3 toVector(Direction direction) noexcept;
vec3i toIntVector(Direction direction) noexcept;
Direction fromIntVector(const vec3i& vector) noexcept;
const char* to_string(Direction direction) noexcept;

/**
 * @param side the backward direction into the camera if looking at the cube's side
 * @param up the the up direction of the camera
 */

Direction left(Direction side, Direction up) noexcept;
Direction right(Direction side, Direction up) noexcept;
bool isLeftTurn(Direction side, Direction from, Direction to) noexcept;
bool isRightTurn(Direction side, Direction from, Direction to) noexcept;
bool isDive(Direction side, Direction to) noexcept;
bool isAscend(Direction side, Direction from) noexcept;

Direction defaultUp(Direction side) noexcept;

/**
 * @brief Converts direction from one view to another
 * Calculates the vector with the same relative position to toSide and toUp as fromDir has to
 * fromSide and fromUp. E.g. if fromDir = left(fromSide, fromUp) then result = left(toSide, toUp).
 * @param fromSide & fromUp the backward and up direction of the origin camera looking at the cube
 * @param toSide & toUp the backward and up direction of the destination camera looking at the cube
 * @param fromDir the direction to convert from "from camera" to "to camera"
 */
Direction remap(Direction fromSide, Direction fromUp, Direction toSide, Direction toUp, Direction fromDir) noexcept;
Direction remapDefaultUp(Direction fromSide, Direction toSide, Direction fromDir) noexcept;

} // namespace s3
#endif