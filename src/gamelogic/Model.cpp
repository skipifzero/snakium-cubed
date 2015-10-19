#include "gamelogic/Model.hpp"

#include <iostream>
#include <new>
#include <random> // std::mt19937_64, std::random_device


#include <sfz/Assert.hpp>

namespace s3 {

// Static functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static Direction calculateFromDirection(Direction fromSide, Direction toSide, Direction dirOnFromSide) noexcept
{
	if (fromSide == toSide) return opposite(dirOnFromSide);
	return fromSide;
}

static std::mt19937_64 createRNGGenerator(void) noexcept
{
	static std::random_device rnd_dev;
	return std::mt19937_64(rnd_dev());
}

static SnakeTile* freeRandomTile(Model& model) noexcept
{
	std::vector<SnakeTile*> freeTiles;
	for (size_t i = 0; i < model.numTiles(); i++) {
		if (model.tilePtr(i)->type == TileType::EMPTY) freeTiles.push_back(model.tilePtr(i));
	}

	if (freeTiles.size() == 0) return nullptr;

	static std::mt19937_64 ms = createRNGGenerator();
	std::uniform_int_distribution<size_t> dist{0, freeTiles.size()-1};
	return freeTiles[dist(ms)];
}

// DirectionInput functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

DirectionInput opposite(DirectionInput direction) noexcept
{
	switch (direction) {
	case DirectionInput::UP: return DirectionInput::DOWN;
	case DirectionInput::DOWN: return DirectionInput::UP;
	case DirectionInput::LEFT: return DirectionInput::RIGHT;
	case DirectionInput::RIGHT: return DirectionInput::LEFT;
	case DirectionInput::DIVE: return DirectionInput::DIVE;
	}
}

// Model: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Model::Model(ModelConfig cfg) noexcept
:
	mCfg(cfg),
	mTileCount{static_cast<size_t>(mCfg.gridWidth*mCfg.gridWidth*6)},
	mCurrentSpeed{cfg.tilesPerSecond}
{
	// +1, last tile is the dead head tile
	mTiles = unique_ptr<SnakeTile[]>{new (std::nothrow) SnakeTile[mTileCount+1]};

	// Start position for snake (head)
	Position headPos;
	headPos.side = Direction::BACKWARD;
	const int16_t mid = static_cast<int16_t>(mCfg.gridWidth/2);
	headPos.e1 = mid;
	headPos.e2 = mid;

	// Head
	SnakeTile* headTile = this->tilePtr(headPos);
	headTile->type = TileType::HEAD;
	headTile->to = Direction::UP;
	headTile->from = Direction::DOWN;
	mHeadPtr = headTile;

	// Pre Head
	Position preHeadPos = prevPosition(headTile);
	SnakeTile* preHeadTile = this->tilePtr(preHeadPos);
	preHeadTile->type = TileType::PRE_HEAD;
	preHeadTile->to = calculateFromDirection(headPos.side, preHeadPos.side, headTile->from);
	preHeadTile->from = opposite(preHeadTile->to);
	mPreHeadPtr = preHeadTile;

	// Tail
	Position tailPos = prevPosition(preHeadTile);
	SnakeTile* tailTile = this->tilePtr(tailPos);
	tailTile->type = TileType::TAIL;
	tailTile->to = calculateFromDirection(preHeadPos.side, tailPos.side, preHeadTile->from);
	tailTile->from = opposite(tailTile->to);
	mTailPtr = tailTile;

	// Dead Head Ptr
	mDeadHeadPtr = &mTiles[mTileCount]; // In range since array holds mTileCount + 1 tiles

	// Object
	SnakeTile* freeTile = freeRandomTile(*this);
	freeTile->type = TileType::OBJECT;
	freeTile->to = defaultUp(tilePosition(freeTile).side);
	freeTile->from = opposite(freeTile->to);
}

// Model: Update methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Model::changeDirection(Direction upDir, DirectionInput direction) noexcept
{
	if (mGameOver) return;

	Direction cubeSide = tilePosition(mHeadPtr).side;
	Direction dir;
	switch (direction) {
	case DirectionInput::UP:
		dir = upDir;
		break;
	case DirectionInput::DOWN:
		dir = opposite(upDir);
		break;
	case DirectionInput::LEFT:
		dir = left(cubeSide, upDir);
		break;
	case DirectionInput::RIGHT:
		dir = right(cubeSide, upDir);
		break;
	case DirectionInput::DIVE:
		dir = opposite(cubeSide);
		break;
	}

	if (dir == mHeadPtr->from) return;
	if (mHeadPtr->to == opposite(cubeSide)) return;
	mHeadPtr->to = dir;
}

void Model::update(float delta, bool* changeOccured) noexcept
{
	if (mGameOver) {
		if (changeOccured != nullptr) *changeOccured = false;
		return;
	}

	mProgress += delta * mCurrentSpeed;
	if (mProgress <= 1.0f) {
		if (changeOccured != nullptr) *changeOccured = false;
		return;
	}
	mProgress -= 1.0f;
	if (changeOccured != nullptr) *changeOccured = true;

	// Calculate the next head position
	Position headPos = tilePosition(mHeadPtr);
	Position nextPos = nextPosition(mHeadPtr);
	SnakeTile* nextHeadPtr = this->tilePtr(nextPos);

	// Check if bonus time is over
	if (mCfg.hasBonus) {
		mBonusTimeLeft -= 1;
		if (mBonusTimeLeft == 0) {
			// TODO: Remove O(n) loop
			SnakeTile* bonusTile = nullptr;
			for (size_t i = 0; i < mTileCount; i++) {
				if (mTiles[i].type == TileType::BONUS_OBJECT) {
					bonusTile = &mTiles[i];
					break;
				}
			}
			if (bonusTile != nullptr) {
				bonusTile->type = TileType::EMPTY;
			}
		}
	}

	// Check if object eaten.
	bool objectEaten = false;
	if (nextHeadPtr->type == TileType::OBJECT) {
		objectEaten = true;
		mScore += static_cast<long>(mCfg.objectValue);
		mTimeSinceBonus += 1;

		SnakeTile* freeTile = freeRandomTile(*this);
		if (freeTile != nullptr) {
			freeTile->type = TileType::OBJECT;
			freeTile->to = defaultUp(tilePosition(freeTile).side);
			freeTile->from = opposite(freeTile->to);
		}

		if (mTimeSinceBonus >= mCfg.bonusFrequency) {
			freeTile = freeRandomTile(*this);
			if (freeTile != nullptr) {
				freeTile->type = TileType::BONUS_OBJECT;
				freeTile->to = defaultUp(tilePosition(freeTile).side);
				freeTile->from = opposite(freeTile->to);
			}
			mTimeSinceBonus = 0;
			mBonusTimeLeft = mCfg.bonusDuration;
		}

		nextHeadPtr->type = TileType::EMPTY;
	}
	else if (nextHeadPtr->type == TileType::BONUS_OBJECT) {
		objectEaten = true;
		mScore += static_cast<long>(mCfg.bonusObjectValue);
		nextHeadPtr->type = TileType::EMPTY;
	}

	// Check if speed should be increased
	if (objectEaten) {
		mCurrentSpeed += mCfg.speedIncreasePerObject;
	}

	// Check if Game Over
	if (nextHeadPtr->type != TileType::EMPTY && nextHeadPtr->type != TileType::TAIL) {
		nextHeadPtr = mDeadHeadPtr;
		mDeadHeadPos = nextPos;
		mGameOver = true;
	}

	// Calculate more next pointers
	Position tailPos = tilePosition(mTailPtr);
	Position nextTailPos = (mTailPtr->type == TileType::TAIL_DIGESTING)
	                       ? tailPos : nextPosition(mTailPtr);
	SnakeTile* nextTailPtr = this->tilePtr(nextTailPos);
	SnakeTile* nextPreHeadPtr = mHeadPtr;

	// Move tail
	if (mTailPtr->type == TileType::TAIL_DIGESTING) {
		nextTailPtr->type = TileType::TAIL;
	} else {
		mTailPtr->type = TileType::EMPTY;
		if (digesting(nextTailPtr->type)) nextTailPtr->type = TileType::TAIL_DIGESTING;
		else nextTailPtr->type = TileType::TAIL;
	}

	// Move pre head
	if (digesting(nextPreHeadPtr->type)) nextPreHeadPtr->type = TileType::PRE_HEAD_DIGESTING;
	else nextPreHeadPtr->type = TileType::PRE_HEAD;
	if (mPreHeadPtr != nextTailPtr) {
		if (digesting(mPreHeadPtr->type)) mPreHeadPtr->type = TileType::BODY_DIGESTING;
		else mPreHeadPtr->type = TileType::BODY;
	}

	// Move head
	if (objectEaten) nextHeadPtr->type = TileType::HEAD_DIGESTING;
	else nextHeadPtr->type = TileType::HEAD;
	nextHeadPtr->from = calculateFromDirection(headPos.side, nextPos.side, nextPreHeadPtr->to);

	// Set heads next direction
	if (opposite(nextHeadPtr->from) != nextPos.side) {
		nextHeadPtr->to = opposite(nextHeadPtr->from);
	} else {
		nextHeadPtr->to = nextPreHeadPtr->from;
	}

	// Update pointers
	mHeadPtr = nextHeadPtr;
	mPreHeadPtr = nextPreHeadPtr;
	mTailPtr = nextTailPtr;
}

void Model::updateSetProgress(float progress, bool* changeOccured) noexcept
{
	sfz_assert_debug(mProgress <= progress && progress <= 1.0f);
	this->update((progress - mProgress) / mCurrentSpeed, changeOccured);
}

bool Model::isChangingDirection(Direction upDir, DirectionInput direction) noexcept
{
	Direction oldHeadTo = mHeadPtr->to;
	this->changeDirection(upDir, direction);
	bool isChange = mHeadPtr->to != oldHeadTo;
	mHeadPtr->to = oldHeadTo;
	return isChange;
}

// Model: Access methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

const SnakeTile* Model::tilePtr(Position pos) const noexcept
{
	sfz_assert_debug(mTiles != nullptr);
	sfz_assert_debug(0 <= pos.e1);
	sfz_assert_debug(0 <= pos.e2);
	sfz_assert_debug(0 <= static_cast<uint8_t>(pos.side));
	sfz_assert_debug(pos.e1 < mCfg.gridWidth);
	sfz_assert_debug(pos.e2 < mCfg.gridWidth);
	sfz_assert_debug(static_cast<uint8_t>(pos.side) <= 5);

	const size_t sideSize = mCfg.gridWidth * mCfg.gridWidth;
	SnakeTile* ptr =  (&mTiles[0]) + static_cast<uint8_t>(pos.side)*sideSize + pos.e2*mCfg.gridWidth + pos.e1;

	sfz_assert_debug(ptr < (ptr + mTileCount));

	return ptr;
}

SnakeTile* Model::tilePtr(Position pos) noexcept
{
	const SnakeTile* ptr = ((const Model*)this)->tilePtr(pos);
	return const_cast<SnakeTile*>(ptr);
}

Position Model::tilePosition(const SnakeTile* tilePtr) const noexcept
{
	Position pos;
	size_t length = tilePtr - (&mTiles[0]);

	sfz_assert_debug(length < mTileCount);

	const size_t sideSize = mCfg.gridWidth * mCfg.gridWidth;
	size_t sideOffset = length % sideSize;
	pos.side = static_cast<Direction>((length-sideOffset)/sideSize);

	pos.e1 = sideOffset % mCfg.gridWidth;
	pos.e2 = (sideOffset-pos.e1)/mCfg.gridWidth;

	sfz_assert_debug(0 <= pos.e1);
	sfz_assert_debug(0 <= pos.e2);
	sfz_assert_debug(0 <= static_cast<uint8_t>(pos.side));
	sfz_assert_debug(pos.e1 < mCfg.gridWidth);
	sfz_assert_debug(pos.e2 < mCfg.gridWidth);
	sfz_assert_debug(static_cast<uint8_t>(pos.side) <= 5);

	return pos;
}

// Model: Private methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Position Model::adjacent(Position pos, Direction to) const noexcept
{
	const int gridWidth = mCfg.gridWidth;
	Direction currentSide = pos.side;
	Direction currentUp = defaultUp(currentSide);

	Position newPos = pos;
	Direction toSide;

	if (to == currentUp) { // UP
		newPos.e2 += 1;
		if (newPos.e2 < gridWidth) return newPos;
		toSide = currentUp;
	} else if (to == opposite(currentUp)) { // DOWN
		newPos.e2 -= 1;
		if (newPos.e2 >= 0) return newPos;
		toSide = opposite(currentUp);
	} else if (to == left(currentSide, currentUp)) { // LEFT
		newPos.e1 -= 1;
		if (newPos.e1 >= 0) return newPos;
		toSide = left(currentSide, currentUp);
	} else if (to == right(currentSide, currentUp)) { // RIGHT
		newPos.e1 += 1;
		if (newPos.e1 < gridWidth) return newPos;
		toSide = right(currentSide, currentUp);
	} else if (to == opposite(currentSide)) {

		toSide = opposite(currentSide);
		newPos.side = toSide;

		Direction fromE1 = direction(currentSide, Coordinate::e1);
		Direction fromE2 = direction(currentSide, Coordinate::e2);

		Direction toE1 = direction(to, Coordinate::e1);
		Direction toE2 = direction(to, Coordinate::e2);

		if (fromE1 == toE1 && fromE2 == toE2) {
			return newPos;
		}
		else if (fromE1 == opposite(toE1) && fromE2 == toE2) {
			newPos.e1 = mCfg.gridWidth - newPos.e1 - 1;
		}
		else if (fromE1 == toE1 && fromE2 == opposite(toE2)) {
			newPos.e2 = mCfg.gridWidth - newPos.e2 - 1;
		}
		else if (fromE1 == opposite(toE1) && fromE2 == opposite(toE2)) {
			newPos.e1 = mCfg.gridWidth - newPos.e1 - 1;
			newPos.e2 = mCfg.gridWidth - newPos.e2 - 1;
		}
		else if (fromE1 == toE2 && fromE2 == toE1) {
			std::swap(newPos.e1, newPos.e2);
		}
		else if (fromE1 == opposite(toE2) && fromE2 == toE1) {
			std::swap(newPos.e1, newPos.e2);
			newPos.e1 = mCfg.gridWidth - newPos.e1 - 1;
		}
		else if (fromE1 == toE2 && fromE2 == opposite(toE1)) {
			std::swap(newPos.e1, newPos.e2);
			newPos.e2 = mCfg.gridWidth - newPos.e2 - 1;
		}
		else if (fromE1 == opposite(toE2) && fromE2 == opposite(toE1)) {
			std::swap(newPos.e1, newPos.e2);
			newPos.e1 = mCfg.gridWidth - newPos.e1 - 1;
			newPos.e2 = mCfg.gridWidth - newPos.e2 - 1;
		}
		return newPos;

	} else {
		sfz_error("Should not happen.");
	}

	Direction newDir = opposite(currentSide);
	Coordinate fromDirCoord = coordinate(currentSide, to);
	Coordinate toDirCoord = coordinate(toSide, newDir);

	// Set side
	newPos.side = toSide;

	// Fix the primary coordinate
	if (direction(toSide, toDirCoord) == newDir) newPos.setCoordinate(toDirCoord, 0);
	else newPos.setCoordinate(toDirCoord, gridWidth-1);

	// Fix the secondary coordinate
	if (fromDirCoord == toDirCoord) {
		Coordinate otherCoord = other(toDirCoord);
		if (coordinateSign(pos.side, otherCoord) != coordinateSign(toSide, otherCoord)) {
			newPos.setCoordinate(otherCoord, (gridWidth-1)-pos.coordinate(otherCoord));
		}
	} else {
		Coordinate otherFromCoord = toDirCoord;
		Coordinate otherToCoord = fromDirCoord;
		if (coordinateSign(pos.side, otherFromCoord) != coordinateSign(toSide, otherToCoord)) {
			newPos.setCoordinate(otherToCoord, (gridWidth-1)-pos.coordinate(otherFromCoord));
		} else {
			newPos.setCoordinate(otherToCoord, pos.coordinate(otherFromCoord));
		}
	}

	return newPos;
}

Position Model::nextPosition(const SnakeTile* tile) const noexcept
{
	return this->adjacent(this->tilePosition(tile), tile->to);
}

Position Model::prevPosition(const SnakeTile* tile) const noexcept
{
	return this->adjacent(this->tilePosition(tile), tile->from);
}

} // namespace s3