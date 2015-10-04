#include "gamelogic/Model.hpp"

#include <new>

namespace s3 {

namespace {

Direction2D convertSideDirection(Direction3D from, Direction3D to, Direction2D fromDir) noexcept
{
	if (from == to) return fromDir;
	return unMapDefaultUp(to, opposite(from));
}

std::mt19937_64 createRNGGenerator(void) noexcept
{
	static std::random_device rnd_dev;
	return std::mt19937_64(rnd_dev());
}

SnakeTile* freeRandomTile(Model& model) noexcept
{
	std::vector<SnakeTile*> freeTiles;
	for (size_t i = 0; i < model.mTileCount; i++) {
		if (model.tilePtr(i)->type == TileType::EMPTY) freeTiles.push_back(model.tilePtr(i));
	}

	if (freeTiles.size() == 0) return nullptr;

	static std::mt19937_64 ms = createRNGGenerator();
	std::uniform_int_distribution<size_t> dist{0, freeTiles.size()-1};
	return freeTiles[dist(ms)];
}

} // anonymous namespace

// Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Model::Model(ModelConfig cfg) noexcept
:
	mCfg(cfg),
	mTileCount{static_cast<size_t>(mCfg.gridWidth*mCfg.gridWidth*6)},
	mTiles{new (std::nothrow) SnakeTile[mTileCount+1]}, // +1, last tile is the dead head tile
	mCurrentSpeed{cfg.tilesPerSecond}
{
	// Start position for snake (head)
	Position tempPos;
	tempPos.side = Direction3D::SOUTH;
	const int16_t mid = static_cast<int16_t>(mCfg.gridWidth/2);
	tempPos.e1 = mid;
	tempPos.e2 = mid;

	// Head
	SnakeTile* tile = this->tilePtr(tempPos);
	tile->type = TileType::HEAD;
	tile->to = Direction2D::UP;
	tile->from = Direction2D::DOWN;
	mHeadPtr = tile;

	// Pre Head
	tempPos = adjacent(tempPos, Direction2D::DOWN);
	tile = this->tilePtr(tempPos);
	tile->type = TileType::PRE_HEAD;
	tile->to = Direction2D::UP;
	tile->from = Direction2D::DOWN;
	mPreHeadPtr = tile;

	// Tile
	tempPos = adjacent(tempPos, Direction2D::DOWN);
	tile = this->tilePtr(tempPos);
	tile->type = TileType::TAIL;
	tile->to = Direction2D::UP;
	tile->from = Direction2D::DOWN;
	mTailPtr = tile;

	// Dead Head Ptr
	mDeadHeadPtr = &mTiles[mTileCount]; // In range since array holds mTileCount + 1 tiles

	// Object
	freeRandomTile(*this)->type = TileType::OBJECT;
}

// Update methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Model::changeDirection(Direction3D upDir, Direction2D direction) noexcept
{
	if (mGameOver) return;

	Direction3D cubeSide = tilePosition(mHeadPtr).side;
	Direction3D realDir = map(cubeSide, upDir, direction);
	Direction2D remappedDir = unMapDefaultUp(cubeSide, realDir);

	if (remappedDir == mHeadPtr->from) return;
	mHeadPtr->to = remappedDir;
}

void Model::update(float delta) noexcept
{
	if (mGameOver) return;

	mProgress += delta * mCurrentSpeed;
	if (mProgress <= 1.0f) return;
	mProgress -= 1.0f;

	// Calculate the next head position
	Position headPos = tilePosition(mHeadPtr);
	Position nextPos = adjacent(headPos, mHeadPtr->to);
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
		if (freeTile != nullptr) freeTile->type = TileType::OBJECT;

		if (mTimeSinceBonus >= mCfg.bonusFrequency) {
			freeTile = freeRandomTile(*this);
			if (freeTile != nullptr) freeTile->type = TileType::BONUS_OBJECT;
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
	                       ? tailPos : adjacent(tailPos, mTailPtr->to);
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
	nextHeadPtr->from = (opposite(
	              convertSideDirection(headPos.side, nextPos.side, nextPreHeadPtr->to)));
	nextHeadPtr->to = opposite(nextHeadPtr->from);

	// Update pointers
	mHeadPtr = nextHeadPtr;
	mPreHeadPtr = nextPreHeadPtr;
	mTailPtr = nextTailPtr;
}

// Access methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

const SnakeTile* Model::tilePtr(Position pos) const noexcept
{
#if 0
	const size_t sideSize = mCfg.gridWidth * mCfg.gridWidth;
	return mTiles + static_cast<uint8_t>(pos.side)*sideSize + pos.e2*mCfg.gridWidth + pos.e1;
#else

	assert(0 <= pos.e1);
	assert(0 <= pos.e2);
	assert(0 <= static_cast<uint8_t>(pos.side));
	assert(pos.e1 < mCfg.gridWidth);
	assert(pos.e2 < mCfg.gridWidth);
	assert(static_cast<uint8_t>(pos.side) <= 5);

	const size_t sideSize = mCfg.gridWidth * mCfg.gridWidth;
	SnakeTile* ptr =  (&mTiles[0]) + static_cast<uint8_t>(pos.side)*sideSize + pos.e2*mCfg.gridWidth + pos.e1;

	assert(ptr < (ptr + mTileCount));

	return ptr;
#endif
}

SnakeTile* Model::tilePtr(Position pos) noexcept
{
#if 0
	const size_t sideSize = mCfg.gridWidth * mCfg.gridWidth;
	return mTiles + static_cast<uint8_t>(pos.side)*sideSize + pos.e2*mCfg.gridWidth + pos.e1;
#else

	assert(0 <= pos.e1);
	assert(0 <= pos.e2);
	assert(0 <= static_cast<uint8_t>(pos.side));
	assert(pos.e1 < mCfg.gridWidth);
	assert(pos.e2 < mCfg.gridWidth);
	assert(static_cast<uint8_t>(pos.side) <= 5);

	const size_t sideSize = mCfg.gridWidth * mCfg.gridWidth;
	SnakeTile* ptr =  (&mTiles[0]) + static_cast<uint8_t>(pos.side)*sideSize + pos.e2*mCfg.gridWidth + pos.e1;

	assert(ptr < (ptr + mTileCount));

	return ptr;
#endif
}

Position Model::tilePosition(const SnakeTile* tilePtr) const noexcept
{
#if 0
	Position pos;
	size_t length = tilePtr - mTiles;

	const size_t sideSize = mCfg.gridWidth * mCfg.gridWidth;
	size_t sideOffset = length % sideSize;
	pos.side = static_cast<Direction3D>((length-sideOffset)/sideSize);

	pos.e1 = sideOffset % mCfg.gridWidth;
	pos.e2 = (sideOffset-pos.e1)/mCfg.gridWidth;

	return pos;
#else
	Position pos;
	size_t length = tilePtr - (&mTiles[0]);

	assert(length < mTileCount);

	const size_t sideSize = mCfg.gridWidth * mCfg.gridWidth;
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

// Member functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Position Model::adjacent(Position pos, Direction2D to) const noexcept
{
	Position newPos = pos;
	const int gridWidth = mCfg.gridWidth;

	switch (to) {
	case Direction2D::UP:
		newPos.e2 += 1;
		if (newPos.e2 < gridWidth) return newPos;
		break;
	case Direction2D::DOWN:
		newPos.e2 -= 1;
		if (newPos.e2 >= 0) return newPos;
		break;
	case Direction2D::LEFT:
		newPos.e1 -= 1;
		if (newPos.e1 >= 0) return newPos;
		break;
	case Direction2D::RIGHT:
		newPos.e1 += 1;
		if (newPos.e1 < gridWidth) return newPos;
		break;
	}

	Direction3D toSide = mapDefaultUp(pos.side, to);
	Direction3D newDir = opposite(pos.side);
	Coordinate fromDirCoord = coordinate(pos.side, to);
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

} // namespace s3