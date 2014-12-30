#include "gamelogic/Model.hpp"

namespace s3 {

namespace {

Direction2D convertSideDirection(Direction3D from, Direction3D to, Direction2D fromDir) noexcept
{
	if (from == to) return fromDir;
	return unMapDefaultUp(to, opposite(from));
}


Position adjacent(Position pos, Direction2D to, const int gridWidth) noexcept
{
	Position newPos = pos;

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

size_t calculateGridWidth(size_t size) noexcept
{
	if (size < 3) size = 3;
	if (size > 16) size = 16;
	return size;
}

} // anonymous namespace

// Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Model::Model(size_t size) noexcept
:
	mGridWidth{calculateGridWidth(size)},
	mTileCount{mGridWidth*mGridWidth*6},
	mTiles{new SnakeTile[mTileCount]},
	mProgress{0.0f}
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
	const int16_t mid = static_cast<int32_t>(mGridWidth/2);
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
}

Model::~Model() noexcept
{
	delete[] mTiles;
}

// Member functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Model::changeDirection(Direction3D upDir, Direction2D direction) noexcept
{
	Direction3D cubeSide = getTilePosition(mHeadPtr).side;
	Direction3D realDir = map(cubeSide, upDir, direction);
	Direction2D remappedDir = unMapDefaultUp(cubeSide, realDir);

	if (remappedDir == mHeadPtr->from()) return;
	mHeadPtr->setTo(remappedDir);
}

void Model::update(float delta) noexcept
{
	mProgress += delta;
	if (mProgress <= 1.0f) return;
	mProgress -= 1.0f;

	// Calculate the next head position
	Position headPos = getTilePosition(mHeadPtr);
	Position nextPos = adjacent(headPos, mHeadPtr->to(), mGridWidth);

	// Check if Game Over
	SnakeTile* nextPtr = getTilePtr(nextPos);
	if (nextPtr->type() != TileType::EMPTY) return;

	// Move Snake
	mPreHeadPtr->setType(TileType::BODY);
	mPreHeadPtr = mHeadPtr;
	mPreHeadPtr->setType(TileType::PRE_HEAD);
	mHeadPtr = nextPtr;

	mHeadPtr->setType(TileType::HEAD);
	mHeadPtr->setFrom(opposite(
	              convertSideDirection(headPos.side, nextPos.side, mPreHeadPtr->to())));
	mHeadPtr->setTo(opposite(mHeadPtr->from()));

	Position tailNext = adjacent(getTilePosition(mTailPtr), mTailPtr->to(), mGridWidth);
	mTailPtr->setType(TileType::EMPTY);
	mTailPtr = getTilePtr(tailNext);
	mTailPtr->setType(TileType::TAIL);
}

} // namespace s3