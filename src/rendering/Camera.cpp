#include "rendering/Camera.hpp"

#include <sfz/math/MathHelpers.hpp>

namespace s3 {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static int axisCoord(Direction dir) noexcept
{
	switch (dir) {
	case Direction::BACKWARD:
	case Direction::FORWARD:
		return 2;
	case Direction::UP:
	case Direction::DOWN:
		return 1;
	case Direction::RIGHT:
	case Direction::LEFT:
		return 0;
	}
}

/** Returns a value between 0.0 (down) and 1.0 (up) */
static float upProgress(const vec3& posOnCube, Direction posOnCubeSideUpDir) noexcept
{
	vec3 upAxis = toVector(posOnCubeSideUpDir);
	int upAxisCoord = axisCoord(posOnCubeSideUpDir);
	return std::abs(posOnCube[upAxisCoord] + 0.5f*sfz::sum(upAxis));
}

/** Returns a value between 0.0 (left) and 1.0 (right) */
static float rightProgress(const vec3& posOnCube,
                    Direction posOnCubeSide, Direction posOnCubeSideUpDir) noexcept
{
	vec3 rightAxis = toVector(right(posOnCubeSide, posOnCubeSideUpDir));
	int rightAxisCoord = axisCoord(right(posOnCubeSide, posOnCubeSideUpDir));
	return std::abs(posOnCube[rightAxisCoord] + 0.5f*sfz::sum(rightAxis));
}

static vec3 tilePosToVector(const Model& model, const Position& tilePos) noexcept
{
	// +0.5f to get the midpoint of the tile
	const float e1f = static_cast<float>(tilePos.e1) + 0.5f;
	const float e2f = static_cast<float>(tilePos.e2) + 0.5f; 
	const float tileWidth = 1.0f / static_cast<float>(model.config().gridWidth);

	return (e1f * tileWidth - 0.5f) * toVector(direction(tilePos.side, Coordinate::e1)) +
	       (e2f * tileWidth - 0.5f) * toVector(direction(tilePos.side, Coordinate::e2)) +
	       toVector(tilePos.side) * 0.5f;
}

// Camera
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Camera::Camera() noexcept	
{
	mPos = vec3{0,0,0};
	mUp = vec3{0,1,0};
	mUpTarget = vec3{0,1,0};
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
	if (mUpDir != mLastUpDir && model.progress() > 0.5f) mLastUpDir = mUpDir;


	// Calculate the current position on the cube
	const float tileWidth = 1.0f / static_cast<float>(model.config().gridWidth);
	vec3 posOnCube;
	if (model.progress() <= 0.5f) {
		vec3 diff = toVector(model.preHeadPtr()->to) * model.progress() * tileWidth;
		posOnCube = tilePosToVector(model, preHeadPos) + diff;
	} else {
		vec3 diff = toVector(model.headPtr()->from) * (1.0f - model.progress()) * tileWidth;
		posOnCube = tilePosToVector(model, headPos) + diff;
	}

	// Calculates and set camera position
	mPos = normalize(posOnCube)*2.0f;

	Direction posOnCubeSide = preHeadPos.side;
	Direction posOnCubeSideUpDir = mLastUpDir;
	if (std::abs(posOnCube[axisCoord(posOnCubeSide)]) != 0.5f) {
		posOnCubeSide = headPos.side;
		posOnCubeSideUpDir = mUpDir;
	}

	mUpTarget = toVector(posOnCubeSideUpDir);
	if (!sfz::approxEqual(mUp, mUpTarget)) {
		float maxAnglePerSec = (model.currentSpeed()*tileWidth*3.0f) * sfz::PI()/2.0f;
		float angleDiff = sfz::angle(mUp, mUpTarget);
		sfz::vec3 rotAxis = sfz::cross(mUp, mUpTarget);
		float angleToMove = maxAnglePerSec*delta;
		if ((angleDiff-angleToMove) < 0) angleToMove = angleDiff;
		sfz::mat4 rotMat = sfz::rotationMatrix4(rotAxis, angleToMove);
		mUp = sfz::normalize(transformPoint(rotMat, mUp));
	}

	mViewMatrix = sfz::lookAt(mPos, vec3{0.0f}, mUp);

	float upProg = upProgress(posOnCube, posOnCubeSideUpDir);
	float rightProg = rightProgress(posOnCube, posOnCubeSide, posOnCubeSideUpDir);
	float upProgClosest = upProg <= 0.5f ? upProg : (1.0f - upProg);
	float rightProgClosest = rightProg <= 0.5f ? rightProg : (1.0f - rightProg);
	bool upClosest = upProgClosest <= rightProgClosest;

	mSideRenderOrder[0] = opposite(posOnCubeSide); // Back
	if (upProg <= 0.5f) {
		mSideRenderOrder[1] = posOnCubeSideUpDir; // Top
		mSideRenderOrder[upClosest ? 4 : 3] = opposite(posOnCubeSideUpDir); // Bottom
	} else {
		mSideRenderOrder[1] = opposite(posOnCubeSideUpDir); // Bottom
		mSideRenderOrder[upClosest ? 4 : 3] = posOnCubeSideUpDir; // Top
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