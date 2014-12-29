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

struct Model final {
	// Members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	const size_t mGridWidth, mTileCount;
	SnakeTile* const mTiles;
	float mProgress;
	SnakeTile *mHeadPtr, *mPreHeadPtr, *mTailPtr;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Model() = delete;
	Model(const Model&) = delete;
	Model& operator= (const Model&) = delete;

	Model(size_t size) noexcept;
	~Model() noexcept;

	// Member functions
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void changeDirection(Direction3D upDir, Direction2D direction) noexcept;

	void update(float delta) noexcept;

	inline SnakeTile* getTilePtr(const Position& pos) const noexcept;
	inline Position getTilePosition(SnakeTile* tilePtr) const noexcept;
};


inline SnakeTile* Model::getTilePtr(const Position& pos) const noexcept
{
	static const size_t sideSize = mGridWidth * mGridWidth;
	return mTiles + static_cast<uint8_t>(pos.side)*sideSize + pos.e2*mGridWidth + pos.e1;
}

inline Position Model::getTilePosition(SnakeTile* tilePtr) const noexcept
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