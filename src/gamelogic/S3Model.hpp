#pragma once
#ifndef S3_GAME_LOGIC_S3_MODEL_hpp
#define S3_GAME_LOGIC_S3_MODEL_hpp

#include <cstddef> // size_t
#include <cstdint> // uint8_t
#include <cassert>
#include "gamelogic/SnakeTile.hpp"

namespace s3 {


using std::uint8_t;
using std::size_t;

// Enums
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/**
 * @brief Enum representing which side of a cube a tile is one
 * It's assumed that the coordinate system is a right-handed system with positive y being up. I.e.
 * FRONT is in positive z-axis and TOP is in positive y-axis.
 */
enum class CubeSide : uint8_t {
	TOP = 0,
	BOTTOM = 1,
	FRONT = 2,
	BACK = 3,
	LEFT = 4,
	RIGHT = 5
};

// Snakium Cubed Model
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/**
 * Contains an array of bytes, every 3 bytes represent a tile.
 * msb [ [TileType], [x, y], [z, CubeSide] ] lsb
 */
struct S3Model final {
	const size_t mGridWidth, mByteCount;
	uint8_t* const mBytes;
	float progress;

	S3Model(size_t size) noexcept;
	~S3Model() noexcept;
	
	void update(float delta) noexcept;

	inline uint8_t* getBytePtr(CubeSide cubeSide, size_t x, size_t y) noexcept
	{
		static const size_t sideSize = mGridWidth * mGridWidth;
		return mBytes + static_cast<uint8_t>(cubeSide)*sideSize + y*mGridWidth + x;
	}

	S3Model() = delete;
	S3Model(const S3Model&) = delete;
	S3Model& operator= (const S3Model&) = delete;
};


} // namespace s3

#endif