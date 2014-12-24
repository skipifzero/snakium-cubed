#include "gamelogic/S3Model.hpp"

namespace s3 {

namespace {

size_t calculateGridWidth(size_t size) noexcept
{
	if (size < 3) size = 3;
	if (size > 16) size = 16;
	return size;
}

} // anonymous namespace

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
	const size_t mid = mGridWidth/2;

	SnakeTile* tile = getTilePtr(CubeSide::FRONT, mid, 0);
	tile->setType(TileType::TAIL);
	tile->setFrom(TileDirection::DOWN);
	tile->setTo(TileDirection::UP);

	tile = getTilePtr(CubeSide::FRONT, mid, 1);
	tile->setType(TileType::PRE_HEAD);
	tile->setFrom(TileDirection::DOWN);
	tile->setTo(TileDirection::UP);

	tile = getTilePtr(CubeSide::FRONT, mid, 2);
	tile->setType(TileType::HEAD);
	tile->setFrom(TileDirection::DOWN);
	tile->setTo(TileDirection::UP);
}

S3Model::~S3Model() noexcept
{
	delete[] mTiles;
}

void S3Model::update(float delta) noexcept
{
	mProgress += delta;
	if (mProgress <= 1.0f) return;
	mProgress -= 1.0f;

	
}


} // namespace s3