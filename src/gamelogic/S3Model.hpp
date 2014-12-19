#pragma once
#ifndef S3_GAME_LOGIC_S3_MODEL_hpp
#define S3_GAME_LOGIC_S3_MODEL_hpp

#include <cstdint>

namespace s3 {

/*
 * General motivation to the low-level implementation.
 *
 * I'd like to try and reduce cache misses by loading more or less the whole model into the cpu
 * cache. This is still Snake, so the performance isn't really needed. It's mostly just a fun thing
 * I decided to try out.
 */

using uint8_t = std::uint8_t;

/**
 * Enum representing the type of a SnakeTile.
 * Needs 4 bits.
 */
enum class TileType : uint8_t {
	EMPTY = 0,
	OBJECT = 1,
	BONUS_OBJECT = 2,

	HEAD = 3,
	BODY = 4,
	TAIL = 5,

	HEAD_DIGESTING = 6,
	BODY_DIGESTING = 7,
	TAIL_DIGESTING = 8
};

/**
 * Enum representing a direction on a SnakeTile.
 * Needs 2 bits.
 */
enum class TileDirection : uint8_t {
	UP = 0,
	DOWN = 1,
	LEFT = 2,
	RIGHT = 3
};

/**
 * bits ==  msb [ 7 6 5 4 3 2 1 0 ] lsb
 * bits 3-0: TileType
 * bits 5-4: from TileDirection
 * bits 7-6: to TileDirection
 */

inline TileType type(uint8_t bits) noexcept
{
	return static_cast<TileType>(bits & 0x0F);
}

inline TileDirection from(uint8_t bits) noexcept
{
	return static_cast<TileDirection>(((bits >> 4) & 0x03));
}

inline TileDirection to(uint8_t bits) noexcept
{
	return static_cast<TileDirection>(((bits >> 6) & 0x03));
}

inline void setType(uint8_t& bits, TileType tileType) noexcept
{
	bits &= 0xF0; // Clear previous type
	bits |= (static_cast<uint8_t>(tileType) & 0x0F); // Set new type
}

inline void setFrom(uint8_t& bits, TileDirection fromDir) noexcept
{
	bits &= 0xCF; // Clear previous from direction
	bits |= ((static_cast<uint8_t>(fromDir) << 4) & 0x30); // Set new from direction
}

inline void setTo(uint8_t& bits, TileDirection toDir) noexcept
{
	bits &= 0x3F; // Clear previous to direction
	bits |= ((static_cast<uint8_t>(toDir) << 6) & 0xC0); // Set new to direction
}


} // namespace s3

#endif