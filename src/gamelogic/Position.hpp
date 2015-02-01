#pragma once
#ifndef S3_GAMELOGIC_POSITION_HPP
#define S3_GAMELOGIC_POSITION_HPP

#include <cstdint> // uint8_t, int16_t
#include <iostream>
#include <exception> // std::terminate
#include "sfz/math/Vector.hpp"
#include "gamelogic/Direction.hpp"

#include "sfz/MSVC12HackON.hpp"

namespace s3 {

using std::uint8_t;
using std::int16_t;

/**
 * @brief Simple enum representing the two different coordinates.
 */
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
	inline int16_t coordinate(Coordinate coord) noexcept;
	inline void setCoordinate(Coordinate coord, int16_t value) noexcept;
};

template<typename T>
inline int sgn(T value) noexcept;

inline Direction3D direction(Direction3D side, Coordinate coordinate) noexcept;
inline sfz::vec3f directionVector(Direction3D side, Coordinate coordinate) noexcept;
inline Coordinate coordinate(Direction3D side, Direction3D dir) noexcept;
inline Coordinate coordinate(Direction3D side, Direction2D dir) noexcept;
inline int coordinateSign(Direction3D side, Coordinate coord) noexcept;
inline int coordinateSign(Direction3D side, Coordinate coord) noexcept;
inline Coordinate other(Coordinate coord) noexcept;

} // namespace s3

#include "sfz/MSVC12HackOFF.hpp"
#include "gamelogic/Position.inl"
#endif