#include "Camera.hpp"

namespace s3 {

namespace {

const sfz::vec3f ZERO{0,0,0};

sfz::vec3f transformPoint(const sfz::mat4f& transformation, const sfz::vec3f& point) noexcept
{
	sfz::vec4f point4{point[0], point[1], point[2], 1.0f};
	point4 = transformation * point4;
	return sfz::vec3f{point4[0], point4[1], point4[2]};
}

int axisCoord(const sfz::vec3f& vec) noexcept
{
	if (vec[0] != 0.0f) return 0;
	if (vec[1] != 0.0f) return 1;
	if (vec[2] != 0.0f) return 2;
	return -1;
}

int axisCoord(Direction3D dir) noexcept
{
	switch (dir) {
	case Direction3D::NORTH: return 2;
	case Direction3D::SOUTH: return 2;
	case Direction3D::WEST: return 0;
	case Direction3D::EAST: return 0;
	case Direction3D::UP: return 1;
	case Direction3D::DOWN: return 1;
	}
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
	Position headPos = model.getTilePosition(model.mHeadPtr);
	Position preHeadPos = model.getTilePosition(model.mPreHeadPtr);

	// Update up direction and lastCubeSide
	if (mLastCubeSide != headPos.side) {
		if (headPos.side == mUpDir) mUpDir = opposite(mLastCubeSide);
		else if (headPos.side == opposite(mUpDir)) mUpDir = mLastCubeSide;
		mLastCubeSide = headPos.side;
		std::cout << headPos.side << ", upDir: " << mUpDir << "\n";
	}
	if (mUpDir != mLastUpDir && model.mProgress > 0.5f) mLastUpDir = mUpDir;


	// Calculate the current position on the cube
	const float tileWidth = 1.0f / static_cast<float>(model.mCfg.gridWidth);
	sfz::vec3f posOnCube;
	if (model.mProgress <= 0.5f) {
		Direction3D preHeadTo = mapDefaultUp(preHeadPos.side, model.mPreHeadPtr->to());
		sfz::vec3f diff = toVector(preHeadTo) * model.mProgress * tileWidth;
		posOnCube = tilePosToVector(model, preHeadPos) + diff;
	} else {
		Direction3D headFrom = mapDefaultUp(headPos.side, model.mHeadPtr->from());
		sfz::vec3f diff = toVector(headFrom) * (1.0f - model.mProgress) * tileWidth;
		posOnCube = tilePosToVector(model, headPos) + diff;
	}

	// Calculates and set camera position
	mPos = posOnCube.normalize()*2.0f;


	Direction3D posOnCubeSide = preHeadPos.side;
	Direction3D posOnCubeSideUpDir = mLastUpDir;
	if (std::abs(posOnCube[axisCoord(posOnCubeSide)]) != 0.5f) {
		posOnCubeSide = headPos.side;
		posOnCubeSideUpDir = mUpDir;
	}
	//std::cout << "progress: " << model.mProgress << ", headSide:" << headPos.side << ", preSide: " << preHeadPos.side << ", posOnCubeSide: " << posOnCubeSide << std::endl;


	// So, given posOnCube, posOnCubeSide and posOnCubeSideUpDir, what can we accomplish?

	sfz::mat4f rot = sfz::rotationMatrix(toVector(posOnCubeSideUpDir), sfz::g_PI_FLOAT/2.0f);

	int upAxisCoord = axisCoord(posOnCubeSideUpDir);
	int rightAxisCoord = axisCoord(right(posOnCubeSide, posOnCubeSideUpDir));

	sfz::vec3f temp = posOnCube;
	float upFactor = std::abs(posOnCube[rightAxisCoord])/0.5f;
	temp[upAxisCoord] *= upFactor;

	sfz::vec3f rightSidePos = transformPoint(rot, temp);

	mUp = sfz::cross(posOnCube, rightSidePos);

	std::cout << "\nside: " << posOnCubeSide << ", up: " << posOnCubeSideUpDir
	          << "\nposOnCube: " << posOnCube << ", temp: " << temp << ", upFactor: " << upFactor
			  << "\nrightSidePos: " << rightSidePos << ", mUp: " << mUp << std::endl;


	/*sfz::vec3f upAxis = toVector(posOnCubeSideUpDir);
	sfz::vec3f rightAxis = toVector(right(posOnCubeSide, posOnCubeSideUpDir));
	int upAxisCoord = axisCoord(posOnCubeSideUpDir);
	int rightAxisCoord = axisCoord(right(posOnCubeSide, posOnCubeSideUpDir));
	float upSideProgress = posOnCube[upAxisCoord] + 0.5f*upAxis.sum();
	float rightSideProgress = posOnCube[rightAxisCoord] + 0.5f*rightAxis.sum();

	//std::cout << "upProg: " << upSideProgress << ", rightProg: " << rightSideProgress << std::endl;

	Direction3D targetSideUp = opposite(posOnCubeSide);
	Direction3D targetSideRight = right(posOnCubeSideUpDir, targetSideUp);
	sfz::vec3f targetSideUpAxis = toVector(targetSideUp);
	sfz::vec3f targetSideRightAxis = toVector(targetSideRight);


	float upProgAbs = std::abs(upSideProgress);
	float rightProgAbs = std::abs(rightSideProgress);
	float diff;
	if (rightProgAbs <= 0.5f) {
		diff = rightProgAbs + 2.0f*(0.5f-rightProgAbs)*upProgAbs;
	} else {
		diff = rightProgAbs - 2.0f*(rightProgAbs-0.5f)*upProgAbs;
	}
	//sfz::vec3f upTargetPos = toVector(posOnCubeSideUpDir);
	sfz::vec3f upTargetPos = toVector(posOnCubeSideUpDir) * 0.5f
	                       + (upProgAbs - 0.5f) * targetSideUpAxis
						   + (diff - 0.5f) * targetSideRightAxis;
						   //+ (std::abs(rightSideProgress) - 0.5f) * targetSideRightAxis;
	std::cout << "\nside: " << posOnCubeSide << ", up: " << posOnCubeSideUpDir << ", upProg: " << upSideProgress << ", rightProg: " << rightSideProgress
	          << "\ntargetUp:  " << targetSideUpAxis << ", targetRight: " << targetSideRightAxis
	          << "\nposOnCube: " << posOnCube << ", upTargetPos: " << upTargetPos << std::endl;

	mUp = upTargetPos - posOnCube;*/
	mViewMatrix = sfz::lookAt(mPos, ZERO, mUp);
}

} // namespace s3