#pragma once
#ifndef S3_GAMELOGIC_POSITION_HPP
#define S3_GAMELOGIC_POSITION_HPP

#include <cstdint> // uint8_t
#include "sfz/math/Vector.hpp"
#include "gamelogic/Direction.hpp"

namespace s3 {

using std::uint8_t;
using std::int16_t;

/**
 * @brief Struct representing the position of a SnakeTile.
 * Mapping:
 * e1: x if possible, otherwise z
 * e2: y if possible, otherwise z
 */
struct Position final {
	Direction3D side;
	int16_t e1, e2;
};

/**
 * @brief Simple enum representing the two different coordinates.
 */
enum class Coordinate : uint8_t {
	e1 = 0,
	e2 = 1
};

inline Direction3D direction(Direction3D side, Coordinate coordinate) noexcept;
inline sfz::vec3f directionVector(Direction3D side, Coordinate coordinate) noexcept;

} // namespace s3

#include "gamelogic/Position.inl"
#endif