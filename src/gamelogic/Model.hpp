#pragma once
#ifndef S3_GAME_LOGIC_S3_MODEL_hpp
#define S3_GAME_LOGIC_S3_MODEL_hpp

#include <cstddef> // size_t
#include <cstdint> // uint8_t
#include <memory>
#include <vector>

#include "gamelogic/ModelConfig.hpp"
#include "gamelogic/Object.hpp"
#include "gamelogic/Direction.hpp"
#include "gamelogic/Position.hpp"
#include "gamelogic/SnakeTile.hpp"
#include "gamelogic/Stats.hpp"

namespace s3 {

using std::int32_t;
using std::int64_t;
using std::size_t;
using std::uint8_t;
using std::unique_ptr;
using std::vector;

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
	bool isChangingDirection(Direction upDir, DirectionInput direction) noexcept;

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
	inline bool isGameOver() const noexcept { return mGameOver; }
	inline float currentSpeed() const noexcept { return mCurrentSpeed; }
	inline const Stats& stats() const noexcept { return mStats; }

	inline const vector<Object>& objects() const noexcept { return mObjects; }
	inline const bool hasTimeShiftBonus() const noexcept { return mShiftTimeLeft > 0; }

private:
	// Private methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Position adjacent(Position pos, Direction to) const noexcept;
	Position nextPosition(const SnakeTile* tile) const noexcept;
	Position prevPosition(const SnakeTile* tile) const noexcept;

	void addObject() noexcept;
	void addBonusObject() noexcept;
	void updateObjects() noexcept;

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

	vector<Object> mObjects;

	float mProgress = 0.0f;
	bool mGameOver = false;
	float mCurrentSpeed = 0.0f;
	int32_t mTimeSinceBonus = 0;
	int32_t mShiftTimeLeft = 0;

	Stats mStats;
};

} // namespace s3
#endif