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
}


} // anonymous namespace

void Camera::update(Direction3D upDir, const sfz::vec3f& tilePos) noexcept
{
	mPos = tilePos.normalize()*2.0f;
	mUp = s3::toVector(upDir);
	
	mViewMatrix = sfz::lookAt(mPos, ZERO, mUp);
}

} // namespace s3