#pragma once
#ifndef S3_GAME_LOGIC_S3_MODEL_hpp
#define S3_GAME_LOGIC_S3_MODEL_hpp

#include <cstddef> // size_t
#include <cstdint> // uint8_t
#include <cassert>
#include "gamelogic/Direction.hpp"
#include "gamelogic/Position.hpp"
#include "gamelogic/SnakeTile.hpp"
#include <iostream>

namespace s3 {

using std::uint8_t;
using std::size_t;

struct TilePosition {
	Direction3D cubeSide;
	int x, y;
};

struct S3Model final {
	// Members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	const size_t mGridWidth, mTileCount;
	SnakeTile* const mTiles;
	float mProgress;
	SnakeTile *mHeadPtr, *mPreHeadPtr, *mTailPtr;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	S3Model() = delete;
	S3Model(const S3Model&) = delete;
	S3Model& operator= (const S3Model&) = delete;

	S3Model(size_t size) noexcept;
	~S3Model() noexcept;

	// Member functions
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void changeDirection(Direction3D upDir, Direction2D direction) noexcept;

	void update(float delta) noexcept;

	inline SnakeTile* getTilePtr(Direction3D cubeSide, size_t x, size_t y) noexcept
	{
		static const size_t sideSize = mGridWidth * mGridWidth;
		return mTiles + static_cast<uint8_t>(cubeSide)*sideSize + y*mGridWidth + x;
	}

	TilePosition getTilePosition(SnakeTile* tilePtr) noexcept;
};

} // namespace s3

#endif