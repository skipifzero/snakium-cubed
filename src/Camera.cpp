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

sfz::vec3f tilePosToVector(const Model& model, const Position& tilePos) noexcept
{
	// +0.5f to get the midpoint of the tile
	const float e1f = static_cast<float>(tilePos.e1) + 0.5f;
	const float e2f = static_cast<float>(tilePos.e2) + 0.5f; 
	const float tileWidth = 1.0f / static_cast<float>(model.mCfg.gridWidth);

	return (e1f * tileWidth - 0.5f) * directionVector(tilePos.side, Coordinate::e1) +
	       (e2f * tileWidth - 0.5f) * directionVector(tilePos.side, Coordinate::e2) +
	       toVector(tilePos.side) * 0.5f;
}

bool approxEqual(float lhs, float rhs) noexcept
{
	float eps = 0.001f;
	return lhs <= rhs + eps && lhs >= rhs - eps;
}

bool approxEqual(const sfz::vec3f& lhs, const sfz::vec3f& rhs) noexcept
{
	if (!approxEqual(lhs[0], rhs[0])) return false;
	if (!approxEqual(lhs[1], rhs[1])) return false;
	if (!approxEqual(lhs[2], rhs[2])) return false;
	return true;
}

} // anonymous namespace

Camera::Camera() noexcept	
{
	mPos = sfz::vec3f{0,0,0};
	mUp = sfz::vec3f{0,1,0};
	mUpTarget = sfz::vec3f{0,1,0};
}

void Camera::update(const Model& model, float delta) noexcept
{
	Position headPos = model.getTilePosition(model.mHeadPtr);
	Position preHeadPos = model.getTilePosition(model.mPreHeadPtr);

	// Update up direction and lastCubeSide
	if (mLastCubeSide != headPos.side) {
		if (headPos.side == mUpDir) mUpDir = opposite(mLastCubeSide);
		else if (headPos.side == opposite(mUpDir)) mUpDir = mLastCubeSide;
		mLastCubeSide = headPos.side;
		//std::cout << headPos.side << ", upDir: " << mUpDir << "\n";
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

	mUpTarget = toVector(posOnCubeSideUpDir);
	if (!approxEqual(mUp, mUpTarget)) {
		float maxAnglePerSec = (model.mCfg.tilesPerSecond*tileWidth*2.5f) * sfz::g_PI_FLOAT/2.0f;
		float angleDiff = sfz::angle(mUp, mUpTarget);
		sfz::vec3f rotAxis = sfz::cross(mUp, mUpTarget);
		float angleToMove = maxAnglePerSec*delta;
		if ((angleDiff-angleToMove) < 0) angleToMove = angleDiff;
		sfz::mat4f rotMat = sfz::rotationMatrix(rotAxis, angleToMove);
		mUp = transformPoint(rotMat, mUp).normalize();
	}

	mViewMatrix = sfz::lookAt(mPos, ZERO, mUp);
}

} // namespace s3