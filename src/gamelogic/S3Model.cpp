#include "gamelogic/S3Model.hpp"

namespace s3 {

namespace {

struct TilePosition {
	CubeSide cubeSide;
	int x, y;
};

TilePosition getPosition(S3Model& model, SnakeTile* tilePtr) noexcept
{
	TilePosition result;
	size_t length = tilePtr - model.mTiles;

	static const size_t sideSize = model.mGridWidth * model.mGridWidth;
	size_t sideOffset = length % sideSize;
	result.cubeSide = static_cast<CubeSide>((length-sideOffset)/sideSize);

	size_t x = sideOffset % model.mGridWidth;
	size_t y = (sideOffset-x)/model.mGridWidth;
	result.x = x;
	result.y = y;

	return result;
}

TilePosition adjacent(TilePosition pos, TileDirection to, const int gridWidth) noexcept
{
	TilePosition adjPos = pos;

	switch (to) {

	case TileDirection::UP:
		adjPos.y += 1;
		if (adjPos.y < gridWidth) break;
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

	case TileDirection::DOWN:
		adjPos.y -= 1;
		if (adjPos.y >= 0) break;
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
	tailPtr = tile;
	assert(getPosition(*this, tile).cubeSide == CubeSide::FRONT);
	assert(getPosition(*this, tile).x == mid);
	assert(getPosition(*this, tile).y == 0);

	tile = getTilePtr(CubeSide::FRONT, mid, 1);
	tile->setType(TileType::PRE_HEAD);
	tile->setFrom(TileDirection::DOWN);
	tile->setTo(TileDirection::UP);
	preHeadPtr = tile;
	assert(getPosition(*this, tile).cubeSide == CubeSide::FRONT);
	assert(getPosition(*this, tile).x == mid);
	assert(getPosition(*this, tile).y == 1);

	tile = getTilePtr(CubeSide::FRONT, mid, 2);
	tile->setType(TileType::HEAD);
	tile->setFrom(TileDirection::DOWN);
	tile->setTo(TileDirection::UP);
	headPtr = tile;
	assert(getPosition(*this, tile).cubeSide == CubeSide::FRONT);
	assert(getPosition(*this, tile).x == mid);
	assert(getPosition(*this, tile).y == 2);
}

S3Model::~S3Model() noexcept
{
	delete[] mTiles;
}

// Member functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void S3Model::changeDirection(TileDirection direction) noexcept
{
	if (direction == headPtr->from()) return;
	headPtr->setTo(direction);
}

void S3Model::update(float delta) noexcept
{
	mProgress += delta;
	if (mProgress <= 1.0f) return;
	mProgress -= 1.0f;

	// Calculate the next head position
	TilePosition headPos = getPosition(*this, headPtr);
	TilePosition nextPos = adjacent(headPos, headPtr->to(), mGridWidth);

	// Check if Game Over
	SnakeTile* nextPtr = getTilePtr(nextPos.cubeSide, nextPos.x, nextPos.y);
	if (nextPtr->type() != TileType::EMPTY) return;

	// Move Snake
	preHeadPtr->setType(TileType::BODY);
	preHeadPtr = headPtr;
	preHeadPtr->setType(TileType::PRE_HEAD);
	headPtr = nextPtr;
	headPtr->setType(TileType::HEAD);
	headPtr->setFrom(opposite(preHeadPtr->to())); // TODO: This will not work in cubed world. =/
	headPtr->setTo(opposite(headPtr->from()));

	TilePosition tailNext = adjacent(getPosition(*this, tailPtr), tailPtr->to(), mGridWidth);
	tailPtr->setType(TileType::EMPTY);
	tailPtr = getTilePtr(tailNext.cubeSide, tailNext.x, tailNext.y);
	tailPtr->setType(TileType::TAIL);

}

} // namespace s3