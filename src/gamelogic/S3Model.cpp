#include "gamelogic/S3Model.hpp"

namespace s3 {

namespace {

Position adjacent(Position pos, Direction2D to, const int gridWidth) noexcept
{
	Position adjPos = pos;

	switch (to) {

	case Direction2D::UP:
		adjPos.e2 += 1;
		if (adjPos.e2 < gridWidth) break;
		switch (adjPos.side) {
		case Direction3D::UP:
			adjPos.side = Direction3D::NORTH;
			adjPos.e2 = gridWidth-1;
			break;
		case Direction3D::DOWN:
			break;
		case Direction3D::SOUTH:
			adjPos.side = Direction3D::UP;
			adjPos.e2 = 0;
			break;
		case Direction3D::NORTH:
			break;
		case Direction3D::WEST:
			break;
		case Direction3D::EAST:
			break;
		}
		break;

	case Direction2D::DOWN:
		adjPos.e2 -= 1;
		if (adjPos.e2 >= 0) break;
		switch (adjPos.side) {
		case Direction3D::UP:
			break;
		case Direction3D::DOWN:
			adjPos.side = Direction3D::SOUTH;
			adjPos.e2 = 0;
			break;
		case Direction3D::SOUTH:
			break;
		case Direction3D::NORTH:
			adjPos.side = Direction3D::DOWN;
			adjPos.e2 = gridWidth-1;
			break;
		case Direction3D::WEST:
			break;
		case Direction3D::EAST:
			break;
		}
		break;

	case Direction2D::LEFT:
		adjPos.e1 -= 1;
		if (adjPos.e1 >= 0) break;
		switch (adjPos.side) {
		case Direction3D::UP:
			break;
		case Direction3D::DOWN:
			break;
		case Direction3D::SOUTH:
			break;
		case Direction3D::NORTH:
			break;
		case Direction3D::WEST:
			break;
		case Direction3D::EAST:
			break;
		}
		break;

	case Direction2D::RIGHT:
		adjPos.e1 += 1;
		if (adjPos.e1 < gridWidth) break;
		switch (adjPos.side) {
		case Direction3D::UP:
			break;
		case Direction3D::DOWN:
			break;
		case Direction3D::SOUTH:
			break;
		case Direction3D::NORTH:
			break;
		case Direction3D::WEST:
			break;
		case Direction3D::EAST:
			break;
		}
		break;
	}

	return adjPos;
}

Direction2D convertSideDirection(Direction3D from, Direction3D to, Direction2D fromDir) noexcept
{
	if (from == to) return fromDir;

	switch (from) {
	case Direction3D::UP:
		switch (to) {
		case Direction3D::DOWN:
		case Direction3D::SOUTH:
		case Direction3D::NORTH: return opposite(fromDir);
		case Direction3D::WEST:
		case Direction3D::EAST:
			break;
		}
	case Direction3D::DOWN:
		switch (to) {
		case Direction3D::UP:
		case Direction3D::SOUTH: return opposite(fromDir);
		case Direction3D::NORTH:
		case Direction3D::WEST:
		case Direction3D::EAST:
			break;
		}
	case Direction3D::SOUTH:
		switch (to) {
		case Direction3D::UP: break;
		case Direction3D::DOWN:
		case Direction3D::NORTH:
		case Direction3D::WEST:
		case Direction3D::EAST:
			break;
		}
	case Direction3D::NORTH:
		switch (to) {
		case Direction3D::UP:
		case Direction3D::DOWN: break;
		case Direction3D::SOUTH:
		case Direction3D::WEST:
		case Direction3D::EAST:
			break;
		}
	case Direction3D::WEST:
		switch (to) {
		case Direction3D::UP:
		case Direction3D::DOWN:
		case Direction3D::SOUTH:
		case Direction3D::NORTH:
		case Direction3D::EAST:
			break;
		}
	case Direction3D::EAST:
		switch (to) {
		case Direction3D::UP:
		case Direction3D::DOWN:
		case Direction3D::SOUTH:
		case Direction3D::NORTH:
		case Direction3D::WEST:
			break;
		}
	}
	return fromDir;
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

S3Model::S3Model(size_t size) noexcept
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

S3Model::~S3Model() noexcept
{
	delete[] mTiles;
}

// Member functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void S3Model::changeDirection(Direction3D upDir, Direction2D direction) noexcept
{
	Direction3D cubeSide = getTilePosition(mHeadPtr).side;
	Direction3D realDir = map(cubeSide, upDir, direction);
	Direction2D remappedDir = unMapDefaultUp(cubeSide, realDir);

	if (remappedDir == mHeadPtr->from()) return;
	mHeadPtr->setTo(remappedDir);
}

void S3Model::update(float delta) noexcept
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