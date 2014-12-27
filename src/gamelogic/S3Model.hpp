#pragma once
#ifndef S3_GAME_LOGIC_S3_MODEL_hpp
#define S3_GAME_LOGIC_S3_MODEL_hpp

#include <cstddef> // size_t
#include <cstdint> // uint8_t
#include <cassert>
#include "gamelogic/TileDirection.hpp"
#include "gamelogic/SnakeTile.hpp"
#include <iostream>

namespace s3 {

using std::uint8_t;
using std::size_t;

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

struct TilePosition {
	CubeSide cubeSide;
	int x, y;
};

class S3Model final {
public:
	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	const size_t mGridWidth, mTileCount;
	SnakeTile* const mTiles;
	float mProgress;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	S3Model() = delete;
	S3Model(const S3Model&) = delete;
	S3Model& operator= (const S3Model&) = delete;

	S3Model(size_t size) noexcept;
	~S3Model() noexcept;

	// Member functions
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void changeDirection(TileDirection direction) noexcept;

	void update(float delta) noexcept;

	inline SnakeTile* getTilePtr(CubeSide cubeSide, size_t x, size_t y) noexcept
	{
		static const size_t sideSize = mGridWidth * mGridWidth;
		return mTiles + static_cast<uint8_t>(cubeSide)*sideSize + y*mGridWidth + x;
	}

	TilePosition getHeadPosition(void) noexcept;

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	SnakeTile* headPtr;
	SnakeTile* preHeadPtr;
	SnakeTile* tailPtr;
};

} // namespace s3

#endif