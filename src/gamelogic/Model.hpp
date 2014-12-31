#pragma once
#ifndef S3_GAME_LOGIC_S3_MODEL_hpp
#define S3_GAME_LOGIC_S3_MODEL_hpp

#include <cstddef> // size_t
#include <cstdint> // uint8_t
#include <cassert>
#include <vector>
#include <random> // std::mt19937_64, std::random_device
#include "gamelogic/Config.hpp"
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

	const Config mCfg;
	const size_t mTileCount;
	SnakeTile* const mTiles;
	float mProgress = 0.0f;
	long mScore = 0;
	SnakeTile *mHeadPtr, *mPreHeadPtr, *mTailPtr;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Model() = delete;
	Model(const Model&) = delete;
	Model& operator= (const Model&) = delete;

	Model(Config cfg) noexcept;
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
	static const size_t sideSize = mCfg.gridWidth * mCfg.gridWidth;
	return mTiles + static_cast<uint8_t>(pos.side)*sideSize + pos.e2*mCfg.gridWidth + pos.e1;
}

inline Position Model::getTilePosition(SnakeTile* tilePtr) const noexcept
{
	Position pos;
	size_t length = tilePtr - mTiles;

	static const size_t sideSize = mCfg.gridWidth * mCfg.gridWidth;
	size_t sideOffset = length % sideSize;
	pos.side = static_cast<Direction3D>((length-sideOffset)/sideSize);

	pos.e1 = sideOffset % mCfg.gridWidth;
	pos.e2 = (sideOffset-pos.e1)/mCfg.gridWidth;

	return pos;
}

} // namespace s3

#endif