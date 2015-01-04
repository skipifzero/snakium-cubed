#include "Camera.hpp"

namespace s3 {

namespace {

const sfz::vec3f ZERO{0,0,0};

int axisCoord(const sfz::vec3f& vec) noexcept
{
	if (vec[0] != 0.0f) return 0;
	if (vec[1] != 0.0f) return 1;
	if (vec[2] != 0.0f) return 2;
	return -1;
}

sfz::vec3f calculateUpVector(s3::Direction3D side, const sfz::vec3f& tileVecPos) noexcept
{
	//sfz::vec3f upAxis = s3::toVector(upDir);
	//sfz::vec3f sideAxis = s3::toVector(s3::right(side, upDir));
	/*int upAxisCoord = axisCoord(upAxis);
	int sideAxisCoord = axisCoord(sideAxis);

	sfz::vec3f midCamPos = s3::toVector(side);
	sfz::vec3f upTileVecPos = tileVecPos;
	upTileVecPos[sideAxisCoord] = 0.0f;
	sfz::vec3f sideTileVecPos = tileVecPos;
	sideTileVecPos[upAxisCoord] = 0.0f;

	float upAngle = sfz::angle(midCamPos, upTileVecPos);
	float sideAngle = sfz::angle(midCamPos, sideTileVecPos);

	float upInterp = ((sfz::g_PI_FLOAT/2.0f) - upAngle) / (sfz::g_PI_FLOAT/2.0f);
	float sideInterp = ((sfz::g_PI_FLOAT/2.0f) - sideAngle) / (sfz::g_PI_FLOAT/2.0f);*/

	/*sfz::vec3f upVector = sfz::cross(tileVecPos, sideAxis);
	//sfz::vec3f upVector = sfz::cross(tileVecPos, sfz::cross(upAxis, tileVecPos));
	std::cout << "upVector: " << upVector << std::endl;
	return upVector;*/
	return sfz::vec3f{};
}

sfz::vec3f tilePosToVector(const s3::Model& model, const s3::Position& tilePos) noexcept
{
	// +0.5f to get the midpoint of the tile
	const float e1f = static_cast<float>(tilePos.e1) + 0.5f;
	const float e2f = static_cast<float>(tilePos.e2) + 0.5f; 
	const float tileWidth = 1.0f / static_cast<float>(model.mCfg.gridWidth);

	return (e1f * tileWidth - 0.5f) * s3::directionVector(tilePos.side, s3::Coordinate::e1) +
	       (e2f * tileWidth - 0.5f) * s3::directionVector(tilePos.side, s3::Coordinate::e2) +
	       s3::toVector(tilePos.side) * 0.5f;
}

} // anonymous namespace

void Camera::update(const Model& model) noexcept
{
	auto headPos = model.getTilePosition(model.mHeadPtr);
	auto preHeadPos = model.getTilePosition(model.mPreHeadPtr);

	if (lastCubeSide != headPos.side) {
		if (headPos.side == mUpDir) mUpDir = s3::opposite(lastCubeSide);
		else if (headPos.side == opposite(mUpDir)) mUpDir = lastCubeSide;
		lastCubeSide = headPos.side;
		std::cout << headPos.side << std::endl;
	}



	static const float tileWidth = 1.0f / static_cast<float>(model.mCfg.gridWidth);

	// BAD ASSUMPTION! It will move up half a tile above cube when changing side
	s3::Direction3D preHeadTo = mapDefaultUp(preHeadPos.side, model.getTilePtr(preHeadPos)->to());
	sfz::vec3f tilePartialDiff = s3::toVector(preHeadTo) * model.mProgress * tileWidth;
	sfz::vec3f tileVecPos = tilePosToVector(model, preHeadPos) + tilePartialDiff;
	std::cout << tileVecPos << std::endl;

	mPos = tileVecPos.normalize()*2.0f;
	mUp = s3::toVector(mUpDir);



	
	mViewMatrix = sfz::lookAt(mPos, ZERO, mUp);
}

} // namespace s3