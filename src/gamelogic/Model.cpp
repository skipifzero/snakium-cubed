#include "gamelogic/Model.hpp"

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

Model::Model(Config cfg) noexcept
:
	mCfg(cfg),
	mTileCount{static_cast<size_t>(mCfg.gridWidth*mCfg.gridWidth*6)},
	mTiles{new SnakeTile[mTileCount]}
{
	static_assert(sizeof(SnakeTile) <= 1, "SnakeTile is larger than 1 byte.");

	// Set the type of every SnakeTile to EMPTY.
	SnakeTile* current = mTiles;
	SnakeTile* const max = mTiles + mTileCount;
	while (current < max) {
		current->setType(TileType::EMPTY);
		current++;
	}

	// Create the first Snake.
	Position tempPos;
	tempPos.side = Direction3D::SOUTH;
	const int16_t mid = static_cast<int16_t>(mCfg.gridWidth/2);
	tempPos.e1 = mid;


	tempPos.e2 = 0;
	SnakeTile* tile = getTilePtr(tempPos);
	tile->setType(TileType::TAIL);
	tile->setFrom(Direction2D::DOWN);
	tile->setTo(Direction2D::UP);
	mTailPtr = tile;
	assert(getTilePosition(tile).side == Direction3D::SOUTH);
	assert(getTilePosition(tile).e1 == mid);
	assert(getTilePosition(tile).e2 == 0);

	tempPos.e2 = 1;
	tile = getTilePtr(tempPos);
	tile->setType(TileType::PRE_HEAD);
	tile->setFrom(Direction2D::DOWN);
	tile->setTo(Direction2D::UP);
	mPreHeadPtr = tile;
	assert(getTilePosition(tile).side == Direction3D::SOUTH);
	assert(getTilePosition(tile).e1 == mid);
	assert(getTilePosition(tile).e2 == 1);

	tempPos.e2 = 2;
	tile = getTilePtr(tempPos);
	tile->setType(TileType::HEAD);
	tile->setFrom(Direction2D::DOWN);
	tile->setTo(Direction2D::UP);
	mHeadPtr = tile;
	assert(getTilePosition(tile).side == Direction3D::SOUTH);
	assert(getTilePosition(tile).e1 == mid);
	assert(getTilePosition(tile).e2 == 2);

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

	
	SnakeTile* nextPtr = getTilePtr(nextPos);

	bool objectEaten = false;
	// Check if object eaten.
	if (nextPtr->type() == TileType::OBJECT) {
		objectEaten = true;
		mScore += static_cast<long>(mCfg.pointsPerObject);
		SnakeTile* freeTile = freeRandomTile(*this);
		if (freeTile != nullptr) freeTile->setType(TileType::OBJECT);
		nextPtr->setType(TileType::EMPTY);
	}

	// Check if Game Over
	if (nextPtr->type() != TileType::EMPTY) {
		mGameOver = true;
		mProgress = 1.0f;
		return;
	}

	// Move Snake
	// Previous pre_head, now body.
	if (mPreHeadPtr->type() == TileType::PRE_HEAD) mPreHeadPtr->setType(TileType::BODY);
	else mPreHeadPtr->setType(TileType::BODY_DIGESTING);
	// Previous head, now pre_head
	if (mHeadPtr->type() == TileType::HEAD) mHeadPtr->setType(TileType::PRE_HEAD);
	else mHeadPtr->setType(TileType::PRE_HEAD_DIGESTING);

	// Update head & pre_head ptrs
	mPreHeadPtr = mHeadPtr;
	mHeadPtr = nextPtr;

	// Setup new head tile
	mHeadPtr->setType(objectEaten ? TileType::HEAD_DIGESTING : TileType::HEAD);
	mHeadPtr->setFrom(opposite(
	              convertSideDirection(headPos.side, nextPos.side, mPreHeadPtr->to())));
	mHeadPtr->setTo(opposite(mHeadPtr->from()));

	// Move tail if it's not digesting, otherwise finish digesting.
	if (mTailPtr->type() == TileType::TAIL_DIGESTING) {
		mTailPtr->setType(TileType::TAIL);
	} else {
		Position tailNext = adjacent(getTilePosition(mTailPtr), mTailPtr->to());
		mTailPtr->setType(TileType::EMPTY);
		mTailPtr = getTilePtr(tailNext);
		if (mTailPtr->type() == TileType::BODY || mTailPtr->type() == TileType::PRE_HEAD) {
			mTailPtr->setType(TileType::TAIL);
		} else {
			mTailPtr->setType(TileType::TAIL_DIGESTING);
		}	
	}
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