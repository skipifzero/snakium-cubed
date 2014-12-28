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

	inline SnakeTile* getTilePtr(const Position& pos) noexcept;
	inline Position getTilePosition(SnakeTile* tilePtr) noexcept;
};


inline SnakeTile* S3Model::getTilePtr(const Position& pos) noexcept
{
	static const size_t sideSize = mGridWidth * mGridWidth;
	return mTiles + static_cast<uint8_t>(pos.side)*sideSize + pos.e2*mGridWidth + pos.e1;
}

inline Position S3Model::getTilePosition(SnakeTile* tilePtr) noexcept
{
	Position pos;
	size_t length = tilePtr - mTiles;

	static const size_t sideSize = mGridWidth * mGridWidth;
	size_t sideOffset = length % sideSize;
	pos.side = static_cast<Direction3D>((length-sideOffset)/sideSize);

	pos.e1 = sideOffset % mGridWidth;
	pos.e2 = (sideOffset-pos.e1)/mGridWidth;

	return pos;
}

} // namespace s3

#endif