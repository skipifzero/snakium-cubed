#include "rendering/Camera.hpp"

#include <algorithm>

#include <sfz/math/MathHelpers.hpp>

namespace s3 {

using sfz::mat3;

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

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

	mUpDir = Direction::UP;
	mLastCubeSide = Direction::BACKWARD;
	mTargetCamUp = mCamUp;

	mDiveInProgress = false;
	mDiveFixUpDir = false;
	mDiveInvertUpDir = false;
	mDiveTargetCamDir = mCamDir;
	mDiveTargetCamDirRotAxis = mCamUp;

	mViewFrustum = ViewFrustum{vec3{0.0f} + mCamDir*mCamDist, -mCamDir, mCamUp, 60.0f, 1.0f, 0.25f, 5.0f};
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

	// Update View Frustum
	mViewFrustum.setPos(vec3{0.0f} +mCamDir*mCamDist);
	mViewFrustum.setDir(-mCamDir, mCamUp);
}

void Camera::onResize(float fov, float aspect) noexcept
{
	mViewFrustum.setVerticalFov(fov);
	mViewFrustum.setAspectRatio(aspect);
}

// Back-to-front sorting
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

RenderOrder calculateRenderOrder(vec3 camPos) noexcept
{
	vec3 camDir = normalize(-camPos); // Since cube is centered at (0, 0, 0)

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
		sideArray[i].dotProd = sfz::dot(camDir, toVector(sideArray[i].side));
	}

	std::sort(sideArray, sideArray + 6, [](const TempSide& lhs, const TempSide& rhs) {
		return lhs.dotProd < rhs.dotProd;
	});

	RenderOrder tmp;
	for (size_t i = 0; i < 6; ++i) {
		tmp.renderOrder[i] = sideArray[i].side;
		tmp.renderTileFaceFirst[i] = sideArray[i].dotProd >= 0.5f;
	}

	return tmp;
}

} // namespace s3