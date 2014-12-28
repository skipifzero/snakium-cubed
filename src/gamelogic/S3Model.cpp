#include "gamelogic/S3Model.hpp"

namespace s3 {

namespace {

TilePosition adjacent(TilePosition pos, Direction2D to, const int gridWidth) noexcept
{
	TilePosition adjPos = pos;

	switch (to) {

	case Direction2D::UP:
		adjPos.y += 1;
		if (adjPos.y < gridWidth) break;
		switch (adjPos.cubeSide) {
		case Direction3D::UP:
			adjPos.cubeSide = Direction3D::NORTH;
			adjPos.y = gridWidth-1;
			break;
		case Direction3D::DOWN:
			break;
		case Direction3D::SOUTH:
			adjPos.cubeSide = Direction3D::UP;
			adjPos.y = 0;
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
		adjPos.y -= 1;
		if (adjPos.y >= 0) break;
		switch (adjPos.cubeSide) {
		case Direction3D::UP:
			break;
		case Direction3D::DOWN:
			adjPos.cubeSide = Direction3D::SOUTH;
			adjPos.y = 0;
			break;
		case Direction3D::SOUTH:
			break;
		case Direction3D::NORTH:
			adjPos.cubeSide = Direction3D::DOWN;
			adjPos.y = gridWidth-1;
			break;
		case Direction3D::WEST:
			break;
		case Direction3D::EAST:
			break;
		}
		break;

	case Direction2D::LEFT:
		adjPos.x -= 1;
		if (adjPos.x >= 0) break;
		switch (adjPos.cubeSide) {
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
		adjPos.x += 1;
		if (adjPos.x < gridWidth) break;
		switch (adjPos.cubeSide) {
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
	const int mid = mGridWidth/2;

	SnakeTile* tile = getTilePtr(Direction3D::SOUTH, mid, 0);
	tile->setType(TileType::TAIL);
	tile->setFrom(Direction2D::DOWN);
	tile->setTo(Direction2D::UP);
	mTailPtr = tile;
	assert(getTilePosition(tile).cubeSide == Direction3D::SOUTH);
	assert(getTilePosition(tile).x == mid);
	assert(getTilePosition(tile).y == 0);

	tile = getTilePtr(Direction3D::SOUTH, mid, 1);
	tile->setType(TileType::PRE_HEAD);
	tile->setFrom(Direction2D::DOWN);
	tile->setTo(Direction2D::UP);
	mPreHeadPtr = tile;
	assert(getTilePosition(tile).cubeSide == Direction3D::SOUTH);
	assert(getTilePosition(tile).x == mid);
	assert(getTilePosition(tile).y == 1);

	tile = getTilePtr(Direction3D::SOUTH, mid, 2);
	tile->setType(TileType::HEAD);
	tile->setFrom(Direction2D::DOWN);
	tile->setTo(Direction2D::UP);
	mHeadPtr = tile;
	assert(getTilePosition(tile).cubeSide == Direction3D::SOUTH);
	assert(getTilePosition(tile).x == mid);
	assert(getTilePosition(tile).y == 2);
}

S3Model::~S3Model() noexcept
{
	delete[] mTiles;
}

// Member functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void S3Model::changeDirection(Direction3D upDir, Direction2D direction) noexcept
{
	Direction3D cubeSide = getTilePosition(mHeadPtr).cubeSide;
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
	TilePosition headPos = getTilePosition(mHeadPtr);
	TilePosition nextPos = adjacent(headPos, mHeadPtr->to(), mGridWidth);

	// Check if Game Over
	SnakeTile* nextPtr = getTilePtr(nextPos.cubeSide, nextPos.x, nextPos.y);
	if (nextPtr->type() != TileType::EMPTY) return;

	// Move Snake
	mPreHeadPtr->setType(TileType::BODY);
	mPreHeadPtr = mHeadPtr;
	mPreHeadPtr->setType(TileType::PRE_HEAD);
	mHeadPtr = nextPtr;

	mHeadPtr->setType(TileType::HEAD);
	mHeadPtr->setFrom(opposite(
	              convertSideDirection(headPos.cubeSide, nextPos.cubeSide, mPreHeadPtr->to())));
	mHeadPtr->setTo(opposite(mHeadPtr->from()));

	TilePosition tailNext = adjacent(getTilePosition(mTailPtr), mTailPtr->to(), mGridWidth);
	mTailPtr->setType(TileType::EMPTY);
	mTailPtr = getTilePtr(tailNext.cubeSide, tailNext.x, tailNext.y);
	mTailPtr->setType(TileType::TAIL);

}

TilePosition S3Model::getTilePosition(SnakeTile* tilePtr) noexcept
{
	TilePosition result;
	size_t length = tilePtr - mTiles;

	static const size_t sideSize = mGridWidth * mGridWidth;
	size_t sideOffset = length % sideSize;
	result.cubeSide = static_cast<Direction3D>((length-sideOffset)/sideSize);

	size_t x = sideOffset % mGridWidth;
	size_t y = (sideOffset-x)/mGridWidth;
	result.x = x;
	result.y = y;

	return result;
}

} // namespace s3