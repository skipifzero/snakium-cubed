#pragma once
#ifndef S3_GAMELOGIC_POSITION_HPP
#define S3_GAMELOGIC_POSITION_HPP

#include <cstdint> // uint8_t, int16_t

#include <sfz/math/Vector.hpp>

#include "gamelogic/Direction.hpp"

namespace s3 {

using sfz::vec3;
using std::uint8_t;
using std::int16_t;

/** Simple enum representing the two different coordinates. */
enum class Coordinate : uint8_t {
	e1 = 0,
	e2 = 1
};

/**
 * @brief Struct representing the position of a SnakeTile.
 * Mapping:
 * e1: x if possible, otherwise z
 * e2: y if possible, otherwise z
 */
struct Position final {
	Direction3D side;
	int16_t e1, e2;

	int16_t coordinate(Coordinate coord) noexcept;
	void setCoordinate(Coordinate coord, int16_t value) noexcept;
};

Direction3D direction(Direction3D side, Coordinate coordinate) noexcept;
vec3 directionVector(Direction3D side, Coordinate coordinate) noexcept;
Coordinate coordinate(Direction3D side, Direction3D dir) noexcept;
Coordinate coordinate(Direction3D side, Direction2D dir) noexcept;
int coordinateSign(Direction3D side, Coordinate coord) noexcept;
int coordinateSign(Direction3D side, Coordinate coord) noexcept;
Coordinate other(Coordinate coord) noexcept;

} // namespace s3
#endif