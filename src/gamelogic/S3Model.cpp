#include "gamelogic/S3Model.hpp"

namespace s3 {

namespace {

TilePosition adjacent(TilePosition pos, TileDirection to, const int gridWidth) noexcept
{
	TilePosition adjPos = pos;

	switch (to) {

	case TileDirection::UP:
		adjPos.y += 1;
		if (adjPos.y < gridWidth) break;
		switch (adjPos.cubeSide) {
		case CubeSide::TOP:
			adjPos.cubeSide = CubeSide::BACK;
			adjPos.y = gridWidth-1;
			break;
		case CubeSide::BOTTOM:
			break;
		case CubeSide::FRONT:
			adjPos.cubeSide = CubeSide::TOP;
			adjPos.y = 0;
			break;
		case CubeSide::BACK:
			break;
		case CubeSide::LEFT:
			break;
		case CubeSide::RIGHT:
			break;
		}
		break;

	case TileDirection::DOWN:
		adjPos.y -= 1;
		if (adjPos.y >= 0) break;
		switch (adjPos.cubeSide) {
		case CubeSide::TOP:
			break;
		case CubeSide::BOTTOM:
			adjPos.cubeSide = CubeSide::FRONT;
			adjPos.y = 0;
			break;
		case CubeSide::FRONT:
			break;
		case CubeSide::BACK:
			adjPos.cubeSide = CubeSide::BOTTOM;
			adjPos.y = gridWidth-1;
			break;
		case CubeSide::LEFT:
			break;
		case CubeSide::RIGHT:
			break;
		}
		break;

	case TileDirection::LEFT:
		adjPos.x -= 1;
		if (adjPos.x >= 0) break;
		switch (adjPos.cubeSide) {
		case CubeSide::TOP:
			break;
		case CubeSide::BOTTOM:
			break;
		case CubeSide::FRONT:
			break;
		case CubeSide::BACK:
			break;
		case CubeSide::LEFT:
			break;
		case CubeSide::RIGHT:
			break;
		}
		break;

	case TileDirection::RIGHT:
		adjPos.x += 1;
		if (adjPos.x < gridWidth) break;
		switch (adjPos.cubeSide) {
		case CubeSide::TOP:
			break;
		case CubeSide::BOTTOM:
			break;
		case CubeSide::FRONT:
			break;
		case CubeSide::BACK:
			break;
		case CubeSide::LEFT:
			break;
		case CubeSide::RIGHT:
			break;
		}
		break;
	}

	return adjPos;
}

TileDirection convertSideDirection(CubeSide from, CubeSide to, TileDirection fromDir) noexcept
{
	if (from == to) return fromDir;

	switch (from) {
	case CubeSide::TOP:
		switch (to) {
		case CubeSide::BOTTOM:
		case CubeSide::FRONT:
		case CubeSide::BACK: return opposite(fromDir);
		case CubeSide::LEFT:
		case CubeSide::RIGHT:
			break;
		}
	case CubeSide::BOTTOM:
		switch (to) {
		case CubeSide::TOP:
		case CubeSide::FRONT: return opposite(fromDir);
		case CubeSide::BACK:
		case CubeSide::LEFT:
		case CubeSide::RIGHT:
			break;
		}
	case CubeSide::FRONT:
		switch (to) {
		case CubeSide::TOP: break;
		case CubeSide::BOTTOM:
		case CubeSide::BACK:
		case CubeSide::LEFT:
		case CubeSide::RIGHT:
			break;
		}
	case CubeSide::BACK:
		switch (to) {
		case CubeSide::TOP:
		case CubeSide::BOTTOM: break;
		case CubeSide::FRONT:
		case CubeSide::LEFT:
		case CubeSide::RIGHT:
			break;
		}
	case CubeSide::LEFT:
		switch (to) {
		case CubeSide::TOP:
		case CubeSide::BOTTOM:
		case CubeSide::FRONT:
		case CubeSide::BACK:
		case CubeSide::RIGHT:
			break;
		}
	case CubeSide::RIGHT:
		switch (to) {
		case CubeSide::TOP:
		case CubeSide::BOTTOM:
		case CubeSide::FRONT:
		case CubeSide::BACK:
		case CubeSide::LEFT:
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

	SnakeTile* tile = getTilePtr(CubeSide::FRONT, mid, 0);
	tile->setType(TileType::TAIL);
	tile->setFrom(TileDirection::DOWN);
	tile->setTo(TileDirection::UP);
	mTailPtr = tile;
	assert(getTilePosition(tile).cubeSide == CubeSide::FRONT);
	assert(getTilePosition(tile).x == mid);
	assert(getTilePosition(tile).y == 0);

	tile = getTilePtr(CubeSide::FRONT, mid, 1);
	tile->setType(TileType::PRE_HEAD);
	tile->setFrom(TileDirection::DOWN);
	tile->setTo(TileDirection::UP);
	mPreHeadPtr = tile;
	assert(getTilePosition(tile).cubeSide == CubeSide::FRONT);
	assert(getTilePosition(tile).x == mid);
	assert(getTilePosition(tile).y == 1);

	tile = getTilePtr(CubeSide::FRONT, mid, 2);
	tile->setType(TileType::HEAD);
	tile->setFrom(TileDirection::DOWN);
	tile->setTo(TileDirection::UP);
	mHeadPtr = tile;
	assert(getTilePosition(tile).cubeSide == CubeSide::FRONT);
	assert(getTilePosition(tile).x == mid);
	assert(getTilePosition(tile).y == 2);
}

S3Model::~S3Model() noexcept
{
	delete[] mTiles;
}

// Member functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void S3Model::changeDirection(TileDirection direction) noexcept
{
	if (direction == mHeadPtr->from()) return;
	mHeadPtr->setTo(direction);
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
	result.cubeSide = static_cast<CubeSide>((length-sideOffset)/sideSize);

	size_t x = sideOffset % mGridWidth;
	size_t y = (sideOffset-x)/mGridWidth;
	result.x = x;
	result.y = y;

	return result;
}

} // namespace s3