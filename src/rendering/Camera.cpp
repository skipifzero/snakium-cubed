#include "Camera.hpp"

namespace s3 {

namespace {

const sfz::vec3 ZERO{0,0,0};

sfz::vec3 transformPoint(const sfz::mat4& transformation, const sfz::vec3& point) noexcept
{
	sfz::vec4 point4{point[0], point[1], point[2], 1.0f};
	point4 = transformation * point4;
	return sfz::vec3{point4[0], point4[1], point4[2]};
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

/** Returns a value between 0.0 (down) and 1.0 (up) */
float upProgress(const sfz::vec3& posOnCube, Direction3D posOnCubeSideUpDir) noexcept
{
	sfz::vec3 upAxis = toVector(posOnCubeSideUpDir);
	int upAxisCoord = axisCoord(posOnCubeSideUpDir);
	return std::abs(posOnCube[upAxisCoord] + 0.5f*sfz::sum(upAxis));
}

/** Returns a value between 0.0 (left) and 1.0 (right) */
float rightProgress(const sfz::vec3& posOnCube,
                    Direction3D posOnCubeSide, Direction3D posOnCubeSideUpDir) noexcept
{
	sfz::vec3 rightAxis = toVector(right(posOnCubeSide, posOnCubeSideUpDir));
	int rightAxisCoord = axisCoord(right(posOnCubeSide, posOnCubeSideUpDir));
	return std::abs(posOnCube[rightAxisCoord] + 0.5f*sfz::sum(rightAxis));
}

sfz::vec3 tilePosToVector(const Model& model, const Position& tilePos) noexcept
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

bool approxEqual(const sfz::vec3& lhs, const sfz::vec3& rhs) noexcept
{
	if (!approxEqual(lhs[0], rhs[0])) return false;
	if (!approxEqual(lhs[1], rhs[1])) return false;
	if (!approxEqual(lhs[2], rhs[2])) return false;
	return true;
}

} // anonymous namespace

Camera::Camera() noexcept	
{
	mPos = sfz::vec3{0,0,0};
	mUp = sfz::vec3{0,1,0};
	mUpTarget = sfz::vec3{0,1,0};
}

void Camera::update(const Model& model, float delta) noexcept
{
	Position headPos = model.tilePosition(model.headPtr());
	Position preHeadPos = model.tilePosition(model.preHeadPtr());

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
	sfz::vec3 posOnCube;
	if (model.mProgress <= 0.5f) {
		Direction3D preHeadTo = mapDefaultUp(preHeadPos.side, model.preHeadPtr()->to);
		sfz::vec3 diff = toVector(preHeadTo) * model.mProgress * tileWidth;
		posOnCube = tilePosToVector(model, preHeadPos) + diff;
	} else {
		Direction3D headFrom = mapDefaultUp(headPos.side, model.headPtr()->from);
		sfz::vec3 diff = toVector(headFrom) * (1.0f - model.mProgress) * tileWidth;
		posOnCube = tilePosToVector(model, headPos) + diff;
	}

	// Calculates and set camera position
	mPos = normalize(posOnCube)*2.0f;

	Direction3D posOnCubeSide = preHeadPos.side;
	Direction3D posOnCubeSideUpDir = mLastUpDir;
	if (std::abs(posOnCube[axisCoord(posOnCubeSide)]) != 0.5f) {
		posOnCubeSide = headPos.side;
		posOnCubeSideUpDir = mUpDir;
	}

	mUpTarget = toVector(posOnCubeSideUpDir);
	if (!approxEqual(mUp, mUpTarget)) {
		float maxAnglePerSec = (model.mCurrentSpeed*tileWidth*3.0f) * sfz::PI()/2.0f;
		float angleDiff = sfz::angle(mUp, mUpTarget);
		sfz::vec3 rotAxis = sfz::cross(mUp, mUpTarget);
		float angleToMove = maxAnglePerSec*delta;
		if ((angleDiff-angleToMove) < 0) angleToMove = angleDiff;
		sfz::mat4 rotMat = sfz::rotationMatrix4(rotAxis, angleToMove);
		mUp = sfz::normalize(transformPoint(rotMat, mUp));
	}

	mViewMatrix = sfz::lookAt(mPos, ZERO, mUp);

	float upProg = upProgress(posOnCube, posOnCubeSideUpDir);
	float rightProg = rightProgress(posOnCube, posOnCubeSide, posOnCubeSideUpDir);
	float upProgClosest = upProg <= 0.5f ? upProg : (1.0f - upProg);
	float rightProgClosest = rightProg <= 0.5f ? rightProg : (1.0f - rightProg);
	bool upClosest = upProgClosest <= rightProgClosest;

	mSideRenderOrder[0] = opposite(posOnCubeSide); // Back
	if (upProg <= 0.5f) {
		mSideRenderOrder[1] = up(posOnCubeSide, posOnCubeSideUpDir); // Top
		mSideRenderOrder[upClosest ? 4 : 3] = down(posOnCubeSide, posOnCubeSideUpDir); // Bottom
	} else {
		mSideRenderOrder[1] = down(posOnCubeSide, posOnCubeSideUpDir); // Bottom
		mSideRenderOrder[upClosest ? 4 : 3] = up(posOnCubeSide, posOnCubeSideUpDir); // Top
	}
	if (rightProg <= 0.5f) {
		mSideRenderOrder[2] = right(posOnCubeSide, posOnCubeSideUpDir); // Right
		mSideRenderOrder[upClosest ? 3 : 4] = left(posOnCubeSide, posOnCubeSideUpDir); // Left
	} else {
		mSideRenderOrder[2] = left(posOnCubeSide, posOnCubeSideUpDir); // Left
		mSideRenderOrder[upClosest ? 3 : 4] = right(posOnCubeSide, posOnCubeSideUpDir); // Right
	}
	mSideRenderOrder[5] = posOnCubeSide; // Front

	for (size_t i = 0; i < 6; i++) {
		mRenderTileFaceFirst[i] = dot(mPos, toVector(mSideRenderOrder[i])) >= 0.5f;
	}
}

} // namespace s3