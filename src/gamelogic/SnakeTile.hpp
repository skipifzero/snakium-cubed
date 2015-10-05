#pragma once
#ifndef S3_GAMELOGIC_SNAKETILE_HPP
#define S3_GAMELOGIC_SNAKETILE_HPP

#include <cstdint> // uint8_t

#include "gamelogic/Direction.hpp"

namespace s3 {

using std::uint8_t;

/** Enum representing the type of a SnakeTile */
enum class TileType : uint8_t {
	EMPTY = 0,
	OBJECT = 1,
	BONUS_OBJECT = 2,

	HEAD = 3,
	PRE_HEAD = 4,
	BODY = 5,
	TAIL = 6,

	HEAD_DIGESTING = 7,
	PRE_HEAD_DIGESTING = 8,
	BODY_DIGESTING = 9,
	TAIL_DIGESTING = 10
};

bool digesting(TileType type) noexcept;

/** Struct representing a SnakeTile */
struct SnakeTile final {
	SnakeTile() noexcept = default;
	SnakeTile(const SnakeTile&) noexcept = default;
	SnakeTile& operator= (const SnakeTile&) noexcept = default;

	SnakeTile(TileType type, Direction from, Direction to) noexcept;

	TileType type = TileType::EMPTY;
	Direction from = Direction::DOWN;
	Direction to = Direction::UP;
};

} // namespace s3
#endif
