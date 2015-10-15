#pragma once
#ifndef S3_GAME_LOGIC_S3_MODEL_hpp
#define S3_GAME_LOGIC_S3_MODEL_hpp

#include <cstddef> // size_t
#include <cstdint> // uint8_t
#include <memory>

#include "gamelogic/ModelConfig.hpp"
#include "gamelogic/Direction.hpp"
#include "gamelogic/Position.hpp"
#include "gamelogic/SnakeTile.hpp"

namespace s3 {

using std::size_t;
using std::uint8_t;
using std::unique_ptr;

enum class DirectionInput : uint8_t {
	UP, DOWN, LEFT, RIGHT, DIVE
};

class Model final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Model() = delete;
	Model(const Model&) = delete;
	Model& operator= (const Model&) = delete;

	Model(ModelConfig cfg) noexcept;

	// Update methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void changeDirection(Direction upDir, DirectionInput direction) noexcept;
	void update(float delta, bool* changeOccured = nullptr) noexcept;
	void updateSetProgress(float progress, bool* changeOccured = nullptr) noexcept;

	// Access methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline const SnakeTile* tilePtr(size_t index) const noexcept { return &mTiles[index]; }
	inline SnakeTile* tilePtr(size_t index) noexcept { return &mTiles[index]; }
	const SnakeTile* tilePtr(Position pos) const noexcept;
	SnakeTile* tilePtr(Position pos) noexcept;

	Position tilePosition(const SnakeTile* tilePtr) const noexcept;

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline const ModelConfig& config() const noexcept { return mCfg; }

	inline size_t numTiles() const noexcept { return mTileCount; }
	inline const SnakeTile* headPtr() const noexcept { return mHeadPtr; }
	inline const SnakeTile* preHeadPtr() const noexcept { return mPreHeadPtr; }
	inline const SnakeTile* deadHeadPtr() const noexcept { return mDeadHeadPtr; }
	inline Position deadHeadPos() const noexcept { return mDeadHeadPos; }

	inline float progress() const noexcept { return mProgress; }
	inline long score() const noexcept { return mScore; }
	inline bool isGameOver() const noexcept { return mGameOver; }
	inline float currentSpeed() const noexcept { return mCurrentSpeed; }

private:
	// Private methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Position adjacent(Position pos, Direction to) const noexcept;
	Position nextPosition(const SnakeTile* tile) const noexcept;
	Position prevPosition(const SnakeTile* tile) const noexcept;

	// Private Members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	const ModelConfig mCfg;

	unique_ptr<SnakeTile[]> mTiles;
	size_t mTileCount = 0;
	SnakeTile* mHeadPtr = nullptr;
	SnakeTile* mPreHeadPtr = nullptr;
	SnakeTile* mTailPtr = nullptr;
	SnakeTile* mDeadHeadPtr = nullptr;
	Position mDeadHeadPos;

	float mProgress = 0.0f;
	long mScore = 0;
	bool mGameOver = false;
	float mCurrentSpeed = 0.0f;
	int mTimeSinceBonus = 0;
	int mBonusTimeLeft = 0;
};

} // namespace s3
#endif