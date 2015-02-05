#include "gamelogic/Model.hpp"

#include "sfz/MSVC12HackON.hpp"

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

SnakeTile* freeRandomTile(const Model& model) noexcept
{
	std::vector<SnakeTile*> freeTiles;
	for (size_t i = 0; i < model.mTileCount; i++) {
		if (model.mTiles[i].type() == TileType::EMPTY) freeTiles.push_back(model.mTiles + i);
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
	mTiles{new SnakeTile[mTileCount+1]} // +1, last tile is the dead head tile
{
	// Set the type of every SnakeTile to EMPTY.
	SnakeTile* current = mTiles;
	SnakeTile* const max = mTiles + mTileCount + 1; // +1 to reset dead head tile
	while (current < max) {
		current->setType(TileType::EMPTY);
		current++;
	}

	// Start position for snake (head)
	Position tempPos;
	tempPos.side = Direction3D::SOUTH;
	const int16_t mid = static_cast<int16_t>(mCfg.gridWidth/2);
	tempPos.e1 = mid;
	tempPos.e2 = mid;

	// Head
	SnakeTile* tile = getTilePtr(tempPos);
	tile->setType(TileType::HEAD);
	tile->setTo(Direction2D::UP);
	tile->setFrom(Direction2D::DOWN);
	mHeadPtr = tile;

	// Pre Head
	tempPos = adjacent(tempPos, Direction2D::DOWN);
	tile = getTilePtr(tempPos);
	tile->setType(TileType::PRE_HEAD);
	tile->setTo(Direction2D::UP);
	tile->setFrom(Direction2D::DOWN);
	mPreHeadPtr = tile;

	// Tile
	tempPos = adjacent(tempPos, Direction2D::DOWN);
	tile = getTilePtr(tempPos);
	tile->setType(TileType::TAIL);
	tile->setTo(Direction2D::UP);
	tile->setFrom(Direction2D::DOWN);
	mTailPtr = tile;

	// Dead Head Ptr
	mDeadHeadPtr = mTiles + mTileCount;

	// Object
	freeRandomTile(*this)->setType(TileType::OBJECT);
}

Model::~Model() noexcept
{
	delete[] mTiles;
}

// Member functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Model::changeDirection(Direction3D upDir, Direction2D direction) noexcept
{
	if (mGameOver) return;

	Direction3D cubeSide = getTilePosition(mHeadPtr).side;
	Direction3D realDir = map(cubeSide, upDir, direction);
	Direction2D remappedDir = unMapDefaultUp(cubeSide, realDir);

	if (remappedDir == mHeadPtr->from()) return;
	mHeadPtr->setTo(remappedDir);
}

void Model::update(float delta) noexcept
{
	if (mGameOver) return;

	mProgress += delta * mCfg.tilesPerSecond;
	if (mProgress <= 1.0f) return;
	mProgress -= 1.0f;

	// Calculate the next head position
	Position headPos = getTilePosition(mHeadPtr);
	Position nextPos = adjacent(headPos, mHeadPtr->to());
	SnakeTile* nextHeadPtr = getTilePtr(nextPos);

	// Check if bonus time is over
	if (mCfg.hasBonus) {
		mBonusTimeLeft -= 1;
		if (mBonusTimeLeft == 0) {
			// TODO: Remove O(n) loop
			SnakeTile* bonusTile = nullptr;
			for (size_t i = 0; i < mTileCount; i++) {
				if ((mTiles+i)->type() == TileType::BONUS_OBJECT) {
					bonusTile = mTiles+i;
					break;
				}
			}
			if (bonusTile != nullptr) {
				bonusTile->setType(TileType::EMPTY);
			}
		}
	}

	// Check if object eaten.
	bool objectEaten = false;
	if (nextHeadPtr->type() == TileType::OBJECT) {
		objectEaten = true;
		mScore += static_cast<long>(mCfg.pointsPerObject);
		mTimeSinceBonus += 1;

		SnakeTile* freeTile = freeRandomTile(*this);
		if (freeTile != nullptr) freeTile->setType(TileType::OBJECT);

		if (mTimeSinceBonus >= mCfg.bonusFrequency) {
			freeTile = freeRandomTile(*this);
			if (freeTile != nullptr) freeTile->setType(TileType::BONUS_OBJECT);
			mTimeSinceBonus = 0;
			mBonusTimeLeft = mCfg.bonusDuration;
		}

		nextHeadPtr->setType(TileType::EMPTY);
	}
	else if (nextHeadPtr->type() == TileType::BONUS_OBJECT) {
		objectEaten = true;
		mScore += static_cast<long>(mCfg.pointsPerBonusObject);
		nextHeadPtr->setType(TileType::EMPTY);
	}

	// Check if Game Over
	if (nextHeadPtr->type() != TileType::EMPTY && nextHeadPtr->type() != TileType::TAIL) {
		nextHeadPtr = mDeadHeadPtr;
		mDeadHeadPos = nextPos;
		mGameOver = true;
	}

	// Calculate more next pointers
	Position tailPos = getTilePosition(mTailPtr);
	Position nextTailPos = (mTailPtr->type() == TileType::TAIL_DIGESTING)
	                       ? tailPos : adjacent(tailPos, mTailPtr->to());
	SnakeTile* nextTailPtr = getTilePtr(nextTailPos);
	SnakeTile* nextPreHeadPtr = mHeadPtr;

	// Move tail
	if (mTailPtr->type() == TileType::TAIL_DIGESTING) {
		nextTailPtr->setType(TileType::TAIL);
	} else {
		mTailPtr->setType(TileType::EMPTY);
		if (digesting(nextTailPtr->type())) nextTailPtr->setType(TileType::TAIL_DIGESTING);
		else nextTailPtr->setType(TileType::TAIL);
	}

	// Move pre head
	if (digesting(nextPreHeadPtr->type())) nextPreHeadPtr->setType(TileType::PRE_HEAD_DIGESTING);
	else nextPreHeadPtr->setType(TileType::PRE_HEAD);
	if (mPreHeadPtr != nextTailPtr) {
		if (digesting(mPreHeadPtr->type())) mPreHeadPtr->setType(TileType::BODY_DIGESTING);
		else mPreHeadPtr->setType(TileType::BODY);
	}

	// Move head
	if (objectEaten) nextHeadPtr->setType(TileType::HEAD_DIGESTING);
	else nextHeadPtr->setType(TileType::HEAD);
	nextHeadPtr->setFrom(opposite(
	              convertSideDirection(headPos.side, nextPos.side, nextPreHeadPtr->to())));
	nextHeadPtr->setTo(opposite(nextHeadPtr->from()));

	// Update pointers
	mHeadPtr = nextHeadPtr;
	mPreHeadPtr = nextPreHeadPtr;
	mTailPtr = nextTailPtr;
}

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

#include "sfz/MSVC12HackOFF.hpp"