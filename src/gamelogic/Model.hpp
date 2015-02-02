#pragma once
#ifndef S3_GAME_LOGIC_S3_MODEL_hpp
#define S3_GAME_LOGIC_S3_MODEL_hpp

#include <cstddef> // size_t
#include <cstdint> // uint8_t
#include <cassert>
#include <vector>
#include <random> // std::mt19937_64, std::random_device
#include <iostream>

#include "gamelogic/ModelConfig.hpp"
#include "gamelogic/Direction.hpp"
#include "gamelogic/Position.hpp"
#include "gamelogic/SnakeTile.hpp"

#include "sfz/MSVC12HackON.hpp"

namespace s3 {

using std::uint8_t;
using std::size_t;

class Model final {
public:
	// Public Members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	const ModelConfig mCfg;
	const size_t mTileCount;
	SnakeTile* const mTiles;
	SnakeTile *mHeadPtr, *mPreHeadPtr, *mTailPtr;
	float mProgress = 0.0f;
	long mScore = 0;
	bool mGameOver = false;
	float mTransparentTimeLeft = -1.0f;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Model() = delete;
	Model(const Model&) = delete;
	Model& operator= (const Model&) = delete;

	Model(ModelConfig cfg) noexcept;
	~Model() noexcept;

	// Member functions
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void changeDirection(Direction3D upDir, Direction2D direction) noexcept;

	void update(float delta) noexcept;

	Position adjacent(Position pos, Direction2D to) const noexcept;

	inline SnakeTile* getTilePtr(const Position& pos) const noexcept;
	inline Position getTilePosition(SnakeTile* tilePtr) const noexcept;

private:
	// Private Members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	int mTimeSinceBonus = 0;
	int mBonusTimeLeft = 0;
};


inline SnakeTile* Model::getTilePtr(const Position& pos) const noexcept
{
#if 0
	static const size_t sideSize = mCfg.gridWidth * mCfg.gridWidth;
	return mTiles + static_cast<uint8_t>(pos.side)*sideSize + pos.e2*mCfg.gridWidth + pos.e1;
#else

	assert(0 <= pos.e1);
	assert(0 <= pos.e2);
	assert(0 <= static_cast<uint8_t>(pos.side));
	assert(pos.e1 < mCfg.gridWidth);
	assert(pos.e2 < mCfg.gridWidth);
	assert(static_cast<uint8_t>(pos.side) <= 5);

	static const size_t sideSize = mCfg.gridWidth * mCfg.gridWidth;
	SnakeTile* ptr =  mTiles + static_cast<uint8_t>(pos.side)*sideSize + pos.e2*mCfg.gridWidth + pos.e1;

	assert(ptr < (ptr + mTileCount));

	return ptr;
#endif
}

inline Position Model::getTilePosition(SnakeTile* tilePtr) const noexcept
{
#if 0
	Position pos;
	size_t length = tilePtr - mTiles;

	static const size_t sideSize = mCfg.gridWidth * mCfg.gridWidth;
	size_t sideOffset = length % sideSize;
	pos.side = static_cast<Direction3D>((length-sideOffset)/sideSize);

	pos.e1 = sideOffset % mCfg.gridWidth;
	pos.e2 = (sideOffset-pos.e1)/mCfg.gridWidth;

	return pos;
#else
	Position pos;
	size_t length = tilePtr - mTiles;

	assert(length < mTileCount);

	static const size_t sideSize = mCfg.gridWidth * mCfg.gridWidth;
	size_t sideOffset = length % sideSize;
	pos.side = static_cast<Direction3D>((length-sideOffset)/sideSize);

	pos.e1 = sideOffset % mCfg.gridWidth;
	pos.e2 = (sideOffset-pos.e1)/mCfg.gridWidth;

	assert(0 <= pos.e1);
	assert(0 <= pos.e2);
	assert(0 <= static_cast<uint8_t>(pos.side));
	assert(pos.e1 < mCfg.gridWidth);
	assert(pos.e2 < mCfg.gridWidth);
	assert(static_cast<uint8_t>(pos.side) <= 5);

	return pos;
#endif
}

} // namespace s3

#include "sfz/MSVC12HackOFF.hpp"

#endif