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

}

S3Model::~S3Model() noexcept
{
	delete[] mBytes;
}


} // namespace s3