#include "gamelogic/S3Model.hpp"

namespace s3 {

namespace {

size_t calculateAmountOfBytes(size_t size) noexcept
{
	if (size < 2) size = 2;
	if (size > 16) size = 16;
	/*size_t cubeCount = (size-2)*(size-2)*6 + (size*4 + (size-2)*8);
	return cubeCount * 3; // 3 bytes per cube*/
	return size*size*6 /* sides */ * 3; /* bytes per side */
}

} // anonymous namespace

S3Model::S3Model(size_t size) noexcept
:
	mByteCount{calculateAmountOfBytes(size)},
	mBytes{new uint8_t[mByteCount]}
{
	uint8_t* current = mBytes;
	uint8_t* const max = mBytes + mByteCount;

	CubeSide cubeSide = CubeSide::FRONT;
	for (uint8_t x = 0; x < size; x++) {
		for (uint8_t y = 0; y < size; y++) {
			assert((current+2) < max);

			tileSideSetType(current[0], TileType::EMPTY);
			setFourMSBs(current[1], x); // x coordinate
			setFourLSBs(current[1], y); // y coordinate
			setFourMSBs(current[2], 0); // z coordinate
			setFourLSBs(current[2], static_cast<uint8_t>(cubeSide)); // CubeSide

			current += 3;
		}
	}

	cubeSide = CubeSide::BACK;
	for (uint8_t x = 0; x < size; x++) {
		for (uint8_t y = 0; y < size; y++) {
			assert((current+2) < max);

			tileSideSetType(current[0], TileType::EMPTY);
			setFourMSBs(current[1], x); // x coordinate
			setFourLSBs(current[1], y); // y coordinate
			setFourMSBs(current[2], (size-1)); // z coordinate
			setFourLSBs(current[2], static_cast<uint8_t>(cubeSide)); // CubeSide

			current += 3;
		}
	}


	cubeSide = CubeSide::LEFT;
	for (uint8_t z = 0; z < size; z++) {
		for (uint8_t y = 0; y < size; y++) {
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
	for (uint8_t z = 0; z < size; z++) {
		for (uint8_t y = 0; y < size; y++) {
			assert((current+2) < max);

			tileSideSetType(current[0], TileType::EMPTY);
			setFourMSBs(current[1], (size-1)); // x coordinate
			setFourLSBs(current[1], y); // y coordinate
			setFourMSBs(current[2], z); // z coordinate
			setFourLSBs(current[2], static_cast<uint8_t>(cubeSide)); // CubeSide

			current += 3;
		}
	}


	cubeSide = CubeSide::BOTTOM;
	for (uint8_t x = 0; x < size; x++) {
		for (uint8_t z = 0; z < size; z++) {
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
	for (uint8_t x = 0; x < size; x++) {
		for (uint8_t z = 0; z < size; z++) {
			assert((current+2) < max);

			tileSideSetType(current[0], TileType::EMPTY);
			setFourMSBs(current[1], x); // x coordinate
			setFourLSBs(current[1], (size-1)); // y coordinate
			setFourMSBs(current[2], z); // z coordinate
			setFourLSBs(current[2], static_cast<uint8_t>(cubeSide)); // CubeSide

			current += 3;
		}
	}
}

S3Model::~S3Model() noexcept
{
	delete[] mBytes;
}


} // namespace s3