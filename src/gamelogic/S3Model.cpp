#include "gamelogic/S3Model.hpp"

namespace s3 {

namespace {

size_t calculateGridWidth(size_t size) noexcept
{
	if (size < 3) size = 3;
	if (size > 16) size = 16;
	return size;
}

size_t calculateAmountOfBytes(size_t gridWidh) noexcept
{
	/*size_t cubeCount = (size-2)*(size-2)*6 + (size*4 + (size-2)*8);
	return cubeCount * 3; // 3 bytes per cube*/
	return gridWidh*gridWidh*6 /* sides */ * 3; /* bytes per side */
}

} // anonymous namespace

S3Model::S3Model(size_t size) noexcept
:
	mGridWidth{calculateGridWidth(size)},
	mByteCount{calculateAmountOfBytes(mGridWidth)},
	mBytes{new uint8_t[mByteCount]}
{
	uint8_t* current = mBytes;
	uint8_t* const max = mBytes + mByteCount;

	CubeSide cubeSide = CubeSide::FRONT;
	for (uint8_t x = 0; x < mGridWidth; x++) {
		for (uint8_t y = 0; y < mGridWidth; y++) {
			assert((current+2) < max);

			tileSideSetType(current[0], TileType::EMPTY);
			setFourMSBs(current[1], x); // x coordinate
			setFourLSBs(current[1], y); // y coordinate
			setFourMSBs(current[2], (mGridWidth-1)); // z coordinate
			setFourLSBs(current[2], static_cast<uint8_t>(cubeSide)); // CubeSide

			current += 3;
		}
	}

	cubeSide = CubeSide::BACK;
	for (uint8_t x = 0; x < mGridWidth; x++) {
		for (uint8_t y = 0; y < mGridWidth; y++) {
			assert((current+2) < max);

			tileSideSetType(current[0], TileType::EMPTY);
			setFourMSBs(current[1], x); // x coordinate
			setFourLSBs(current[1], y); // y coordinate
			setFourMSBs(current[2], 0); // z coordinate
			setFourLSBs(current[2], static_cast<uint8_t>(cubeSide)); // CubeSide

			current += 3;
		}
	}


	cubeSide = CubeSide::LEFT;
	for (uint8_t z = 0; z < mGridWidth; z++) {
		for (uint8_t y = 0; y < mGridWidth; y++) {
			assert((current+2) < max);

			tileSideSetType(current[0], TileType::EMPTY);
			setFourMSBs(current[1], 0); // x coordinate
			setFourLSBs(current[1], y); // y coordinate
			setFourMSBs(current[2], z); // z coordinate
			setFourLSBs(current[2], static_cast<uint8_t>(cubeSide)); // CubeSide

			current += 3;
		}
	}

	cubeSide = CubeSide::RIGHT;
	for (uint8_t z = 0; z < mGridWidth; z++) {
		for (uint8_t y = 0; y < mGridWidth; y++) {
			assert((current+2) < max);

			tileSideSetType(current[0], TileType::EMPTY);
			setFourMSBs(current[1], (mGridWidth-1)); // x coordinate
			setFourLSBs(current[1], y); // y coordinate
			setFourMSBs(current[2], z); // z coordinate
			setFourLSBs(current[2], static_cast<uint8_t>(cubeSide)); // CubeSide

			current += 3;
		}
	}


	cubeSide = CubeSide::BOTTOM;
	for (uint8_t x = 0; x < mGridWidth; x++) {
		for (uint8_t z = 0; z < mGridWidth; z++) {
			assert((current+2) < max);

			tileSideSetType(current[0], TileType::EMPTY);
			setFourMSBs(current[1], x); // x coordinate
			setFourLSBs(current[1], 0); // y coordinate
			setFourMSBs(current[2], z); // z coordinate
			setFourLSBs(current[2], static_cast<uint8_t>(cubeSide)); // CubeSide

			current += 3;
		}
	}

	cubeSide = CubeSide::TOP;
	for (uint8_t x = 0; x < mGridWidth; x++) {
		for (uint8_t z = 0; z < mGridWidth; z++) {
			assert((current+2) < max);

			tileSideSetType(current[0], TileType::EMPTY);
			setFourMSBs(current[1], x); // x coordinate
			setFourLSBs(current[1], (mGridWidth-1)); // y coordinate
			setFourMSBs(current[2], z); // z coordinate
			setFourLSBs(current[2], static_cast<uint8_t>(cubeSide)); // CubeSide

			current += 3;
		}
	}

	const size_t mid = mGridWidth/2;
	for (uint8_t* ptr = mBytes; ptr < max; ptr += 3) {
		uint8_t xPos = getFourMSBs(ptr[1]);
		uint8_t yPos = getFourLSBs(ptr[1]);
		uint8_t zPos = getFourMSBs(ptr[2]);
		s3::CubeSide cSide = static_cast<s3::CubeSide>(getFourLSBs(ptr[2]));

		if (xPos == mid && cSide == s3::CubeSide::FRONT) {
			if (yPos == 0) {
				tileSideSetFrom(ptr[0], TileDirection::DOWN);
				tileSideSetTo(ptr[0], TileDirection::UP);
				tileSideSetType(ptr[0], TileType::TAIL);
			} else if (yPos == 1) {
				tileSideSetFrom(ptr[0], TileDirection::DOWN);
				tileSideSetTo(ptr[0], TileDirection::UP);
				tileSideSetType(ptr[0], TileType::PRE_HEAD);
			} else if (yPos == 2) {
				tileSideSetFrom(ptr[0], TileDirection::DOWN);
				tileSideSetTo(ptr[0], TileDirection::UP);
				tileSideSetType(ptr[0], TileType::HEAD);
			}
		}
	}

}

S3Model::~S3Model() noexcept
{
	delete[] mBytes;
}

void S3Model::update(float delta) noexcept
{
	// Time for dah logic huh
}


} // namespace s3