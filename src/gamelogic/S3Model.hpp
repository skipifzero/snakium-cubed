#pragma once
#ifndef S3_GAME_LOGIC_S3_MODEL_hpp
#define S3_GAME_LOGIC_S3_MODEL_hpp

#include <cstddef>
#include <cstdint>

namespace s3 {

/*
 * General motivation to the low-level implementation.
 *
 * I'd like to try and reduce cache misses by loading more or less the whole model into the cpu
 * cache. This is still Snake, so the performance isn't really needed. It's mostly just a fun thing
 * I decided to try out.
 */

using std::uint8_t;
using std::size_t;

// Enums
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/**
 * @brief Enum representing the type of a SnakeTile.
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
 * @brief Enum representing a direction on a SnakeTile.
 * Needs 2 bits.
 */
enum class TileDirection : uint8_t {
	UP = 0,
	DOWN = 1,
	LEFT = 2,
	RIGHT = 3
};

/**
 * @brief Enum representing which side of a cube a tile is one
 * It's assumed that the coordinate system is a right-handed system with positive y being up. I.e.
 * FRONT is in positive z-axis and TOP is in positive y-axis.
 * Needs 3 bits.
 */
enum class CubeSide : uint8_t {
	TOP = 0,
	BOTTOM = 1,
	FRONT = 2,
	BACK = 3,
	LEFT = 4,
	RIGHT = 5
};

/**
 * @brief Sets the four least significant bits of a byte.
 * @param bits a reference to the byte to set
 * @param bitsToSet the bits to set, only the 4 LSBs are used
 */
inline void setFourLSBs(uint8_t& bits, uint8_t bitsToSet) noexcept
{
	bits &= 0xF0; // Clear 4 lsb
	bits |= (bitsToSet & 0x0F);
}

/**
 * @brief Sets the four most significant bits of a byte.
 * @param bits a reference to the byte to set
 * @param bitsToSet the bits to set, only the 4 LSBs are used (first shifted)
 */
inline void setFourMSBs(uint8_t& bits, uint8_t bitsToSet) noexcept
{
	bits &= 0x0F; // Clear 4 msb
	bits |= ((bitsToSet << 4) & 0xF0);
}



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

struct S3Model final {
	size_t mByteCount;
	uint8_t* const mBytes;

	S3Model(size_t size) noexcept;
	~S3Model() noexcept;

	S3Model() = delete;
	S3Model(const S3Model&) = delete;
	S3Model& operator= (const S3Model&) = delete;
};


} // namespace s3

#endif