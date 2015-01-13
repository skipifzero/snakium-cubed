#pragma once
#ifndef S3_GAMELOGIC_SNAKETILE_HPP
#define S3_GAMELOGIC_SNAKETILE_HPP

#include <cstddef> // size_t
#include <cstdint> // uint8_t

#include "gamelogic/Direction.hpp"

namespace s3 {

using std::uint8_t;

/**
 * @brief Enum representing the type of a SnakeTile.
 * Needs 4 bits.
 */
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

inline bool digesting(TileType type) noexcept;

/**
 * Struct representing a SnakeTile.
 * Should takes up exactly one byte in memory, unless struct gets padded for some reason.
 * mBits ==  msb [ [to][from][type] ] lsb
 */
struct SnakeTile final {
	uint8_t mBits;

	inline TileType type(void) const noexcept;
	inline Direction2D from(void) const noexcept;
	inline Direction2D to(void) const noexcept;

	inline void setType(TileType type) noexcept;
	inline void setFrom(Direction2D from) noexcept;
	inline void setTo(Direction2D to) noexcept;
};

} // namespace s3

#include "gamelogic/SnakeTile.inl"
#endif
