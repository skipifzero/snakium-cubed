#include "rendering/Camera.hpp"

#include <algorithm>

#include <sfz/math/MathHelpers.hpp>

namespace s3 {

using sfz::mat3;

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/*static size_t axisCoord(Direction dir) noexcept
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
}*/

/** Returns a value between 0.0 (down) and 1.0 (up) */
/*static float upProgress(vec3 posOnCube, Direction posOnCubeSideUpDir) noexcept
{
	vec3 upAxis = toVector(posOnCubeSideUpDir);
	int upAxisCoord = axisCoord(posOnCubeSideUpDir);
	return std::abs(posOnCube[upAxisCoord] + 0.5f*sfz::sum(upAxis));
}*/

/** Returns a value between 0.0 (left) and 1.0 (right) */
/*static float rightProgress(vec3 posOnCube, Direction posOnCubeSide, Direction posOnCubeSideUpDir) noexcept
{
	vec3 rightAxis = toVector(right(posOnCubeSide, posOnCubeSideUpDir));
	int rightAxisCoord = axisCoord(right(posOnCubeSide, posOnCubeSideUpDir));
	return std::abs(posOnCube[rightAxisCoord] + 0.5f*sfz::sum(rightAxis));
}*/

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

// Camera: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Camera::Camera() noexcept	
{
	mCamDir = vec3{0.0f, 0.0f, 1.0f};
	mCamUp = vec3{0.0f, 1.0f, 0.0f};
	mCamDist = 2.0f;

	mFov = 60.0f;
	mAspect = 1.0f;
	mNear = 0.25f;
	mFar = 5.0f;

	mUpDir = Direction::UP;
	mLastCubeSide = Direction::BACKWARD;
	mTargetCamUp = mCamUp;

	mDiveInProgress = false;
	mDiveFixUpDir = false;
	mDiveInvertUpDir = false;
	mDiveTargetCamDir = mCamDir;
	mDiveTargetCamDirRotAxis = mCamUp;
}

// Camera: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Camera::update(Model& model, float delta) noexcept
{
	Position headPos, preHeadPos;
	Direction headTo;
	if (!model.isGameOver()) {
		headPos = model.tilePosition(model.headPtr());
		headTo = model.headPtr()->to;
		preHeadPos = model.tilePosition(model.preHeadPtr());
	} else {
		headPos = model.deadHeadPos();
		headTo = model.deadHeadPtr()->to;
		preHeadPos = model.tilePosition(model.preHeadPtr());
	}

	// Update and calculate target dir and up vector
	Direction currCubeSide = headPos.side;
	if (mLastCubeSide != currCubeSide) {
		
		if (currCubeSide == mUpDir) { // Upwards
			mUpDir = opposite(mLastCubeSide);
			mTargetCamUp = toVector(mUpDir);
		} else if (currCubeSide == opposite(mUpDir)) { // Downwards
			mUpDir = mLastCubeSide;
			mTargetCamUp = toVector(mUpDir);
		} else if (mLastCubeSide == opposite(currCubeSide)) { // Dive
			
			mDiveInProgress = true;
			mDiveFixUpDir = headTo == mUpDir || headTo == opposite(mUpDir);
			mDiveInvertUpDir = headTo == mUpDir;
			if (mDiveFixUpDir) {
				mUpDir = opposite(mUpDir);
				mTargetCamUp = toVector(mUpDir);
				mDiveFixUpDir = true;
			}
			mDiveTargetCamDir = normalize(tilePosToVector(model, headPos));
			mDiveTargetCamDirRotAxis = normalize(toVector(left(preHeadPos.side, opposite(model.preHeadPtr()->from))));
		}
		
		mLastCubeSide = currCubeSide;
	}

	// Calculate the current position on the cube
	if (headPos.side == opposite(preHeadPos.side)) { // Special case where dive was performed

		if (mDiveInProgress) { // Change side		
			if (!sfz::approxEqual(mCamDir, mDiveTargetCamDir)) {

				float diffAngle = sfz::angle(mCamDir - dot(mCamDir, mDiveTargetCamDirRotAxis)*mDiveTargetCamDirRotAxis,
				                            mDiveTargetCamDir - dot(mDiveTargetCamDir, mDiveTargetCamDirRotAxis)*mDiveTargetCamDirRotAxis);
				float anglePerSec = model.currentSpeed() * 1.2f;
				float angleToMove = anglePerSec*delta;
				if ((diffAngle - angleToMove) < 0) {
					angleToMove = diffAngle;
					mDiveInProgress = false;
					model.updateSetProgress(0.75f); // TODO: Ugly hack.
				}
				mat3 rotMat = sfz::rotationMatrix3(mDiveTargetCamDirRotAxis, angleToMove);
				mCamDir = normalize(rotMat * mCamDir);
				if (mDiveFixUpDir) {
					mCamUp = normalize(sfz::cross(mDiveTargetCamDirRotAxis, mCamDir));
					if (mDiveInvertUpDir) mCamUp = -mCamUp;
				}
			} else {
				mDiveInProgress = false;
				model.updateSetProgress(0.75f); // TODO: Ugly hack.
			}
		}

	} else { // Normal case where dive has not been performed
		const float tileWidth = 1.0f / static_cast<float>(model.config().gridWidth);
		if (model.progress() <= 0.5f) {
			vec3 diff = toVector(model.preHeadPtr()->to) * model.progress() * tileWidth;
			mCamDir = normalize(tilePosToVector(model, preHeadPos) + diff);
		} else {
			vec3 diff = toVector(model.headPtr()->from) * (1.0f - model.progress()) * tileWidth;
			mCamDir = normalize(tilePosToVector(model, headPos) + diff);
		}
	}

	// Approach target cam up
	if (!sfz::approxEqual(mCamUp, mTargetCamUp) && !mDiveInProgress) {
		const float tileWidth = 1.0f / static_cast<float>(model.config().gridWidth);
		const float maxAnglePerSec = (model.currentSpeed()*tileWidth*3.0f) * sfz::PI()/2.0f;
		float angleDiff = sfz::angle(mCamUp, mTargetCamUp);
		vec3 rotAxis = sfz::cross(mCamUp, mTargetCamUp);
		if (sfz::approxEqual(rotAxis, vec3{0.0f})) {
			mCamUp = mTargetCamUp;
		} else {
			float angleToMove = maxAnglePerSec*delta;
			if ((angleDiff-angleToMove) < 0) angleToMove = angleDiff;
			mat4 rotMat = sfz::rotationMatrix4(rotAxis, angleToMove);
			mCamUp = normalize(transformDir(rotMat, mCamUp));
		}
	}

	// Calculate matrices
	mViewMatrix = sfz::lookAt(vec3{0.0f} + mCamDir*mCamDist, vec3{0.0f}, mCamUp);
	mProjMatrix = sfz::glPerspectiveProjectionMatrix(mFov, mAspect, mNear, mFar);

	// Calculate transparency order
	struct TempSide {
		Direction side;
		float dotProd;
	};

	TempSide sideArray[6];
	sideArray[0] = {Direction::BACKWARD, 0.0f};
	sideArray[1] = {Direction::FORWARD, 0.0f};
	sideArray[2] = {Direction::UP, 0.0f};
	sideArray[3] = {Direction::DOWN, 0.0f};
	sideArray[4] = {Direction::LEFT, 0.0f};
	sideArray[5] = {Direction::RIGHT, 0.0f};

	for (size_t i = 0; i < 6; ++i) {
		sideArray[i].dotProd = sfz::dot(mCamDir, toVector(sideArray[i].side));
	}

	std::sort(sideArray, sideArray + 6, [](const TempSide& lhs, const TempSide& rhs) {
		return lhs.dotProd < rhs.dotProd;
	});

	for (size_t i = 0; i < 6; ++i) {
		mSideRenderOrder[i] = sideArray[i].side;
		mRenderTileFaceFirst[i] = sideArray[i].dotProd >= 0.5f;
	}


	/*float upProg = upProgress(posOnCube, posOnCubeSideUpDir);
	float rightProg = rightProgress(posOnCube, posOnCubeSide, posOnCubeSideUpDir);
	float upProgClosest = upProg <= 0.5f ? upProg : (1.0f - upProg);
	float rightProgClosest = rightProg <= 0.5f ? rightProg : (1.0f - rightProg);
	bool upClosest = upProgClosest <= rightProgClosest;

	sideRenderOrder[0] = opposite(posOnCubeSide); // Back
	if (upProg <= 0.5f) {
		sideRenderOrder[1] = posOnCubeSideUpDir; // Top
		sideRenderOrder[upClosest ? 4 : 3] = opposite(posOnCubeSideUpDir); // Bottom
	} else {
		sideRenderOrder[1] = opposite(posOnCubeSideUpDir); // Bottom
		sideRenderOrder[upClosest ? 4 : 3] = posOnCubeSideUpDir; // Top
	}
	if (rightProg <= 0.5f) {
		sideRenderOrder[2] = right(posOnCubeSide, posOnCubeSideUpDir); // Right
		sideRenderOrder[upClosest ? 3 : 4] = left(posOnCubeSide, posOnCubeSideUpDir); // Left
	} else {
		sideRenderOrder[2] = left(posOnCubeSide, posOnCubeSideUpDir); // Left
		sideRenderOrder[upClosest ? 3 : 4] = right(posOnCubeSide, posOnCubeSideUpDir); // Right
	}
	sideRenderOrder[5] = posOnCubeSide; // Front

	for (size_t i = 0; i < 6; i++) {
		renderTileFaceFirst[i] = dot(mPos, toVector(sideRenderOrder[i])) >= 0.5f;
	}*/
}

void Camera::onResize(float fov, float aspect) noexcept
{
	mFov = fov;
	mAspect = aspect;
}

} // namespace s3