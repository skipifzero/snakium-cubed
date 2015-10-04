#pragma once
#ifndef S3_GAME_LOGIC_S3_MODEL_hpp
#define S3_GAME_LOGIC_S3_MODEL_hpp

#include <cstddef> // size_t
#include <cstdint> // uint8_t
#include <memory>


#include "gamelogic/ModelConfig.hpp"


#include <cassert>
#include <vector>
#include <random> // std::mt19937_64, std::random_device
#include <iostream>


#include "gamelogic/Direction.hpp"
#include "gamelogic/Position.hpp"
#include "gamelogic/SnakeTile.hpp"

namespace s3 {

using std::size_t;
using std::uint8_t;
using std::unique_ptr;

class Model final {
public:
	// Public Members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	const ModelConfig mCfg;
	const size_t mTileCount;
	//SnakeTile* const mTiles;
	SnakeTile *mHeadPtr, *mPreHeadPtr, *mTailPtr, *mDeadHeadPtr;
	Position mDeadHeadPos;
	float mProgress = 0.0f;
	long mScore = 0;
	bool mGameOver = false;
	float mCurrentSpeed;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Model() = delete;
	Model(const Model&) = delete;
	Model& operator= (const Model&) = delete;

	Model(ModelConfig cfg) noexcept;

	// Update methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void changeDirection(Direction3D upDir, Direction2D direction) noexcept;
	void update(float delta) noexcept;

	// Access methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline const SnakeTile* tilePtr(size_t index) const noexcept { return &mTiles[index]; }
	inline SnakeTile* tilePtr(size_t index) noexcept { return &mTiles[index]; }
	const SnakeTile* tilePtr(Position pos) const noexcept;
	SnakeTile* tilePtr(Position pos) noexcept;

	Position tilePosition(const SnakeTile* tilePtr) const noexcept;

	// Member functions
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Position adjacent(Position pos, Direction2D to) const noexcept;

private:
	// Private Members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	unique_ptr<SnakeTile[]> mTiles;

	int mTimeSinceBonus = 0;
	int mBonusTimeLeft = 0;
};

} // namespace s3
#endif