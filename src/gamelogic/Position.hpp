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
	Direction side;
	int16_t e1, e2;

	int16_t coordinate(Coordinate coord) noexcept;
	void setCoordinate(Coordinate coord, int16_t value) noexcept;
};

bool operator== (const Position& lhs, const Position& rhs) noexcept;
bool operator!= (const Position& lhs, const Position& rhs) noexcept;

Direction direction(Direction side, Coordinate coordinate) noexcept;
Coordinate coordinate(Direction side, Direction dir) noexcept;
int coordinateSign(Direction side, Coordinate coord) noexcept;
Coordinate other(Coordinate coord) noexcept;

} // namespace s3
#endif