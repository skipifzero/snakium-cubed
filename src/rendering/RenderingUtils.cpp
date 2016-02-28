#include "rendering/RenderingUtils.hpp"

#include "GameLogic.hpp"
#include "rendering/Assets.hpp"
#include "rendering/Camera.hpp"
#include "rendering/Materials.hpp"

namespace s3 {

// Rendering helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

SimpleModel& getTileModel(const SnakeTile* tilePtr, Direction side, float progress, bool gameOver) noexcept
{
	Assets& a = Assets::INSTANCE();
	const bool rightTurn = isRightTurn(side, tilePtr->from, tilePtr->to);
	const bool leftTurn = isLeftTurn(side, tilePtr->from, tilePtr->to);
	const bool dive = isDive(side, tilePtr->to);
	const bool ascend = isAscend(side, tilePtr->from);
	const bool frame1 = progress <= 0.5f;

	switch (tilePtr->type) {
	//case s3::TileType::EMPTY:
	case s3::TileType::OBJECT: return a.OBJECT_MODEL;
	case s3::TileType::BONUS_OBJECT: return a.BONUS_OBJECT_MODEL;

	case TileType::HEAD:
		if (frame1) return a.HEAD_D2U_F1_MODEL;
		else return a.HEAD_D2U_F2_MODEL;

	case TileType::PRE_HEAD:
		if (frame1) {
			if (rightTurn) return (!gameOver) ? a.PRE_HEAD_D2R_F1_MODEL : a.DEAD_PRE_HEAD_D2R_F1_MODEL;
			else if (leftTurn) return (!gameOver) ? a.PRE_HEAD_D2L_F1_MODEL : a.DEAD_PRE_HEAD_D2L_F1_MODEL;
			else return (!gameOver) ? a.PRE_HEAD_D2U_F1_MODEL : a.DEAD_PRE_HEAD_D2U_F1_MODEL;
		} else {
			if (rightTurn) return a.BODY_D2R_MODEL;
			else if (leftTurn) return a.BODY_D2L_MODEL;
			else return a.BODY_D2U_MODEL;
		}

	case TileType::BODY:
		if (rightTurn) return a.BODY_D2R_MODEL;
		else if (leftTurn) return a.BODY_D2L_MODEL;
		else return a.BODY_D2U_MODEL;

	case TileType::TAIL:
		if (frame1) {
			if (rightTurn) return a.TAIL_D2R_F1_MODEL;
			else if (leftTurn) return a.TAIL_D2L_F1_MODEL;
			else return a.TAIL_D2U_F1_MODEL;
		} else {
			if (rightTurn) return a.TAIL_D2R_F2_MODEL;
			else if (leftTurn) return a.TAIL_D2L_F2_MODEL;
			else return a.TAIL_D2U_F2_MODEL;
		}

	case TileType::HEAD_DIGESTING:
		if (frame1) return a.HEAD_D2U_DIG_F1_MODEL;
		else return a.HEAD_D2U_DIG_F2_MODEL;

	case TileType::PRE_HEAD_DIGESTING:
		if (frame1) {
			if (rightTurn) return (!gameOver) ? a.PRE_HEAD_D2R_DIG_F1_MODEL : a.DEAD_PRE_HEAD_D2R_DIG_F1_MODEL;
			else if (leftTurn) return (!gameOver) ? a.PRE_HEAD_D2L_DIG_F1_MODEL : a.DEAD_PRE_HEAD_D2L_DIG_F1_MODEL;
			else return (!gameOver) ? a.PRE_HEAD_D2U_DIG_F1_MODEL : a.DEAD_PRE_HEAD_D2U_DIG_F1_MODEL;
		} else {
			if (rightTurn) return a.BODY_D2R_DIG_MODEL;
			else if (leftTurn) return a.BODY_D2L_DIG_MODEL;
			else return a.BODY_D2U_DIG_MODEL;
		}

	case TileType::BODY_DIGESTING:
		if (rightTurn) return a.BODY_D2R_DIG_MODEL;
		else if (leftTurn) return a.BODY_D2L_DIG_MODEL;
		else return a.BODY_D2U_DIG_MODEL;

	case TileType::TAIL_DIGESTING:
		if (frame1) {
			if (rightTurn) return a.TAIL_D2R_DIG_F1_MODEL;
			else if (leftTurn) return a.TAIL_D2L_DIG_F1_MODEL;
			else return a.TAIL_D2U_DIG_F1_MODEL;
		} else {
			if (rightTurn) return a.TAIL_D2R_DIG_F2_MODEL;
			if (leftTurn) return a.TAIL_D2L_DIG_F2_MODEL;
			else return a.TAIL_D2U_DIG_F2_MODEL;
		}
	}

	return a.NOT_FOUND_MODEL;
}

SimpleModel* getTileProjectionModelPtr(const SnakeTile* tilePtr, Direction side, float progress) noexcept
{
	Assets& a = Assets::INSTANCE();
	const bool rightTurn = isRightTurn(side, tilePtr->from, tilePtr->to);
	const bool leftTurn = isLeftTurn(side, tilePtr->from, tilePtr->to);
	const bool dive = isDive(side, tilePtr->to);
	const bool ascend = isAscend(side, tilePtr->from);
	const bool frame1 = progress <= 0.5f;

	switch (tilePtr->type) {
	
	//case s3::TileType::EMPTY:
	//case s3::TileType::OBJECT:
	//case s3::TileType::BONUS_OBJECT:

	case TileType::HEAD:
	case TileType::HEAD_DIGESTING:
		if (frame1) return &a.HEAD_D2U_F1_PROJECTION_MODEL;
		else return &a.HEAD_D2U_F2_PROJECTION_MODEL;

	case TileType::PRE_HEAD:
		if (frame1) {
			if (rightTurn) return &a.PRE_HEAD_D2R_F1_PROJECTION_MODEL;
			else if (leftTurn) return &a.PRE_HEAD_D2L_F1_PROJECTION_MODEL; 
			else return &a.PRE_HEAD_D2U_F1_PROJECTION_MODEL;
			break;
		} else {
			if (rightTurn) return &a.BODY_D2R_PROJECTION_MODEL;
			else if (leftTurn) return &a.BODY_D2L_PROJECTION_MODEL;
			else return &a.BODY_D2U_PROJECTION_MODEL;
		}

	case TileType::BODY:
		if (rightTurn) return &a.BODY_D2R_PROJECTION_MODEL;
		else if (leftTurn) return &a.BODY_D2L_PROJECTION_MODEL;
		else return &a.BODY_D2U_PROJECTION_MODEL;

	case TileType::TAIL:
		if (frame1) {
			if (rightTurn) return &a.TAIL_D2R_F1_PROJECTION_MODEL;
			else if (leftTurn) return &a.TAIL_D2L_F1_PROJECTION_MODEL;
			else return &a.TAIL_D2U_F1_PROJECTION_MODEL;
		} else {
			if (rightTurn) return &a.TAIL_D2R_F2_PROJECTION_MODEL;
			else if (leftTurn) return &a.TAIL_D2L_F2_PROJECTION_MODEL;
			else return &a.TAIL_D2U_F2_PROJECTION_MODEL;
		}

	case TileType::PRE_HEAD_DIGESTING:
		if (frame1) {
			if (rightTurn) return &a.PRE_HEAD_D2R_DIG_F1_PROJECTION_MODEL;
			else if (leftTurn) return &a.PRE_HEAD_D2L_DIG_F1_PROJECTION_MODEL;
			else return &a.PRE_HEAD_D2U_DIG_F1_PROJECTION_MODEL;
			break;
		} else {
			if (rightTurn) return &a.BODY_D2R_DIG_PROJECTION_MODEL;
			else if (leftTurn) return &a.BODY_D2L_DIG_PROJECTION_MODEL;
			else return &a.BODY_D2U_DIG_PROJECTION_MODEL;
		}

	case TileType::BODY_DIGESTING:
		if (rightTurn) return &a.BODY_D2R_DIG_PROJECTION_MODEL;
		else if (leftTurn) return &a.BODY_D2L_DIG_PROJECTION_MODEL;
		else return &a.BODY_D2U_DIG_PROJECTION_MODEL;

	case TileType::TAIL_DIGESTING:
		if (frame1) {
			if (rightTurn) return &a.TAIL_D2R_DIG_F1_PROJECTION_MODEL;
			else if (leftTurn) return &a.TAIL_D2L_DIG_F1_PROJECTION_MODEL;
			else return &a.TAIL_D2U_DIG_F1_PROJECTION_MODEL;
		} else {
			if (rightTurn) return &a.TAIL_D2R_DIG_F2_PROJECTION_MODEL;
			else if (leftTurn) return &a.TAIL_D2L_DIG_F2_PROJECTION_MODEL;
			else return &a.TAIL_D2U_DIG_F2_PROJECTION_MODEL;
		}

	}

	return nullptr;
}

float getTileAngleRad(Direction side, const SnakeTile* tile) noexcept
{
	Direction up = defaultUp(side);
	Direction from = tile->from;

	// Special case when diving/ascending
	if (tile->from == opposite(side)) {
		from = opposite(tile->to);
	}

	float angle = 0.0f;
	if (from == up) {
		angle = 180.0f;
	} else if (from == opposite(up)) {
		angle = 0.0f;
	} else if (from == left(side, up)) {
		angle = -90.0f;
	} else if (from == right(side, up)) {
		angle = 90.0f;
	}

	// Yeah, I dunno. There probably is a pattern here to make it general, but I don't see it.
	switch (side) {
	case Direction::FORWARD:
		angle += 180.0f;
		break;
	case Direction::BACKWARD:
		// Do nothing.
		break;
	case Direction::LEFT:
		angle -= 90.0f;
		break;
	case Direction::RIGHT:
		angle += 90.0f;
		break;
	case Direction::UP:
		angle += 180.0f;
		break;
	case Direction::DOWN:
		// Do nothing.
		break;
	}

	return angle * sfz::DEG_TO_RAD();
}

vec3 tilePosToVector(const Model& model, const Position& tilePos) noexcept
{
	// +0.5f to get the midpoint of the tile
	const float e1f = static_cast<float>(tilePos.e1) + 0.5f;
	const float e2f = static_cast<float>(tilePos.e2) + 0.5f;
	const float tileWidth = 1.0f / static_cast<float>(model.config().gridWidth);

	return (e1f * tileWidth - 0.5f) * toVector(direction(tilePos.side, Coordinate::e1)) +
	       (e2f * tileWidth - 0.5f) * toVector(direction(tilePos.side, Coordinate::e2)) +
	       toVector(tilePos.side) * 0.5f;
}

mat4 tileSpaceRotation(Direction side) noexcept
{
	switch (side) {
	case Direction::BACKWARD: return sfz::xRotationMatrix4(sfz::PI()/2.0f);
	case Direction::FORWARD: return sfz::xRotationMatrix4(-sfz::PI()/2.0f);
	case Direction::UP: return sfz::identityMatrix4<float>();
	case Direction::DOWN: return sfz::xRotationMatrix4(sfz::PI());
	case Direction::RIGHT: return sfz::zRotationMatrix4(-sfz::PI()/2.0f);
	case Direction::LEFT: return sfz::zRotationMatrix4(sfz::PI()/2.0f);
	}
}

uint32_t tileMaterialId(const SnakeTile* tilePtr) noexcept
{
	switch (tilePtr->type) {
	case TileType::OBJECT:
		return MATERIAL_ID_TILE_OBJECT;

	case TileType::BONUS_OBJECT:
		return MATERIAL_ID_TILE_BONUS_OBJECT;

	case TileType::HEAD:
	case TileType::PRE_HEAD:
	case TileType::BODY:
	case TileType::TAIL:
		return MATERIAL_ID_TILE_SNAKE;

	case TileType::HEAD_DIGESTING:
	case TileType::PRE_HEAD_DIGESTING:
	case TileType::BODY_DIGESTING:
	case TileType::TAIL_DIGESTING:
		return MATERIAL_ID_TILE_SNAKE_DIG;

	case TileType::EMPTY:
	default:
		return MATERIAL_ID_DEFAULT;
	}
}

uint32_t tileDecorationMaterialId(const SnakeTile* tilePtr) noexcept
{
	switch (tilePtr->type) {
	case TileType::EMPTY:
	case TileType::OBJECT:
	case TileType::BONUS_OBJECT:
		return MATERIAL_ID_TILE_DECORATION;
	default:
		return MATERIAL_ID_TILE_DECORATION_OCCUPIED;
	}
}

bool isSnake(const SnakeTile* tilePtr) noexcept
{
	switch (tilePtr->type) {
	case TileType::HEAD:
	case TileType::PRE_HEAD:
	case TileType::BODY:
	case TileType::TAIL:
	case TileType::HEAD_DIGESTING:
	case TileType::PRE_HEAD_DIGESTING:
	case TileType::BODY_DIGESTING:
	case TileType::TAIL_DIGESTING:
		return true;
	default:
		return false;
	}
}

bool isObject(const SnakeTile* tilePtr) noexcept
{
	switch (tilePtr->type) {
	case TileType::OBJECT:
	case TileType::BONUS_OBJECT:
		return true;
	default:
		return false;
	}
}

// Render functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void renderBackground(Program& program, const mat4& viewMatrix) noexcept
{
	Assets& assets = Assets::INSTANCE();

	const mat4 skyModelMatrix = sfz::scalingMatrix4(5.0f);
	const mat4 skyNormalMatrix = sfz::inverse(sfz::transpose(viewMatrix * skyModelMatrix));
	gl::setUniform(program, "uModelMatrix", skyModelMatrix);
	gl::setUniform(program, "uNormalMatrix", skyNormalMatrix);
	setUniform(program, "uMaterialId", MATERIAL_ID_SKY);
	gl::setUniform(program, "uBlurWeight", 0.0f);

	assets.SKYSPHERE_MODEL.render();
}

void renderOpaqueSnakeProjection(const Model& model, Program& program, const mat4& viewMatrix) noexcept
{
	Assets& assets = Assets::INSTANCE();

	const mat4 tileScaling = sfz::scalingMatrix4(1.0f / (16.0f * (float)model.config().gridWidth));

	for (size_t i = 0; i < model.numTiles(); ++i) {
		const SnakeTile* tilePtr = model.tilePtr(i);
		Position tilePos = model.tilePosition(tilePtr);

		mat4 tileSpaceRot = tileSpaceRotation(tilePos.side);
		mat4 tileSpaceRotScaling = tileSpaceRot * tileScaling;

		// Calculate base transform
		mat4 transform = tileSpaceRotScaling;
		transform *= sfz::yRotationMatrix4(getTileAngleRad(tilePos.side, tilePtr));
		sfz::translation(transform, tilePosToVector(model, tilePos));

		// Set uniforms
		const mat4 normalMatrix = sfz::inverse(sfz::transpose(viewMatrix * transform));
		gl::setUniform(program, "uModelMatrix", transform);
		gl::setUniform(program, "uNormalMatrix", normalMatrix);

		// Render tile projection
		setUniform(program, "uMaterialId", MATERIAL_ID_TILE_PROJECTION);
		gl::setUniform(program, "uBlurWeight", 0.0f);
		auto* tileProjModelPtr = getTileProjectionModelPtr(tilePtr, tilePos.side, model.progress());
		if (tileProjModelPtr != nullptr) tileProjModelPtr->render();
	}

	// Render dead snake head projection if game over
	if (model.isGameOver()) {
		const SnakeTile* tilePtr = model.deadHeadPtr();
		Position tilePos = model.deadHeadPos();

		mat4 tileSpaceRot = tileSpaceRotation(tilePos.side);
		mat4 tileSpaceRotScaling = tileSpaceRot * tileScaling;

		// Calculate base transform
		mat4 transform = tileSpaceRotScaling;
		transform *= sfz::yRotationMatrix4(getTileAngleRad(tilePos.side, tilePtr));
		sfz::translation(transform, tilePosToVector(model, tilePos));

		// Set uniforms
		const mat4 normalMatrix = sfz::inverse(sfz::transpose(viewMatrix * transform));
		gl::setUniform(program, "uModelMatrix", transform);
		gl::setUniform(program, "uNormalMatrix", normalMatrix);

		// Render tile model
		setUniform(program, "uMaterialId", MATERIAL_ID_TILE_PROJECTION);
		gl::setUniform(program, "uBlurWeight", 0.0f);
		getTileProjectionModelPtr(tilePtr, tilePos.side, model.progress())->render();
	}
}

void renderCube(const Model& model, Program& program, const mat4& viewMatrix) noexcept
{
	Assets& assets = Assets::INSTANCE();

	const mat4 tileScaling = sfz::scalingMatrix4(1.0f / (16.0f * (float)model.config().gridWidth));

	for (size_t i = 0; i < model.numTiles(); ++i) {
		const SnakeTile* tilePtr = model.tilePtr(i);
		Position tilePos = model.tilePosition(tilePtr);

		// Calculate base transform
		mat4 tileSpaceRot = tileSpaceRotation(tilePos.side);
		mat4 tileSpaceRotScaling = tileSpaceRot * tileScaling;
		mat4 transform = tileSpaceRotScaling;
		transform *= sfz::yRotationMatrix4(getTileAngleRad(tilePos.side, tilePtr));
		sfz::translation(transform, tilePosToVector(model, tilePos));

		// Set uniforms
		const mat4 normalMatrix = sfz::inverse(sfz::transpose(viewMatrix * transform));
		gl::setUniform(program, "uModelMatrix", transform);
		gl::setUniform(program, "uNormalMatrix", normalMatrix);

		// Render tile decoration
		gl::setUniform(program, "uBlurWeight", 1.0f);
		setUniform(program, "uMaterialId", tileDecorationMaterialId(tilePtr));
		assets.TILE_DECORATION_MODEL.render();
	}
}

void renderSnake(const Model& model, Program& program, const mat4& viewMatrix) noexcept
{
	Assets& assets = Assets::INSTANCE();
	
	const mat4 tileScaling = sfz::scalingMatrix4(1.0f / (16.0f * (float)model.config().gridWidth));

	gl::setUniform(program, "uBlurWeight", 3.0f);

	for (size_t i = 0; i < model.numTiles(); ++i) {
		const SnakeTile* tilePtr = model.tilePtr(i);
		Position tilePos = model.tilePosition(tilePtr);

		// Skip non-snake tiles
		if (!isSnake(tilePtr)) continue;

		// Calculate base transform
		mat4 tileSpaceRot = tileSpaceRotation(tilePos.side);
		mat4 tileSpaceRotScaling = tileSpaceRot * tileScaling;
		mat4 transform = tileSpaceRotScaling;
		transform *= sfz::yRotationMatrix4(getTileAngleRad(tilePos.side, tilePtr));
		sfz::translation(transform, tilePosToVector(model, tilePos));

		// Set uniforms
		const mat4 normalMatrix = sfz::inverse(sfz::transpose(viewMatrix * transform));
		gl::setUniform(program, "uModelMatrix", transform);
		gl::setUniform(program, "uNormalMatrix", normalMatrix);

		// Render dive & ascend models
		if (isDive(tilePos.side, tilePtr->to)) {
			setUniform(program, "uMaterialId", MATERIAL_ID_TILE_DIVE_ASCEND);
			assets.DIVE_MODEL.render();
		} else if (isAscend(tilePos.side, tilePtr->from) &&
			tilePtr->type != TileType::TAIL && tilePtr->type != TileType::TAIL_DIGESTING) {
			setUniform(program, "uMaterialId", MATERIAL_ID_TILE_DIVE_ASCEND);
			assets.ASCEND_MODEL.render();
		}

		// Render tile model
		setUniform(program, "uMaterialId", tileMaterialId(tilePtr));
		getTileModel(tilePtr, tilePos.side, model.progress(), model.isGameOver()).render();
	}

	// Render dead snake head if game over (opaque)
	if (model.isGameOver()) {
		const SnakeTile* tilePtr = model.deadHeadPtr();
		Position tilePos = model.deadHeadPos();

		// Calculate base transform
		mat4 tileSpaceRot = tileSpaceRotation(tilePos.side);
		mat4 tileSpaceRotScaling = tileSpaceRot * tileScaling;
		mat4 transform = tileSpaceRotScaling;
		transform *= sfz::yRotationMatrix4(getTileAngleRad(tilePos.side, tilePtr));
		sfz::translation(transform, tilePosToVector(model, tilePos));

		// Set uniforms
		const mat4 normalMatrix = sfz::inverse(sfz::transpose(viewMatrix * transform));
		gl::setUniform(program, "uModelMatrix", transform);
		gl::setUniform(program, "uNormalMatrix", normalMatrix);

		// Render dive & ascend models
		if (isDive(tilePos.side, tilePtr->to)) {
			setUniform(program, "uMaterialId", MATERIAL_ID_TILE_DIVE_ASCEND);
			assets.DIVE_MODEL.render();
		} else if (isAscend(tilePos.side, tilePtr->from) &&
			tilePtr->type != TileType::TAIL && tilePtr->type != TileType::TAIL_DIGESTING) {
			setUniform(program, "uMaterialId", MATERIAL_ID_TILE_DIVE_ASCEND);
			assets.ASCEND_MODEL.render();
		}

		// Render tile model
		setUniform(program, "uMaterialId", tileMaterialId(tilePtr));
		getTileModel(tilePtr, tilePos.side, model.progress(), model.isGameOver()).render();
	}
}

void renderObjects(const Model& model, Program& program, const mat4& viewMatrix) noexcept
{
	Assets& assets = Assets::INSTANCE();

	const mat4 tileScaling = sfz::scalingMatrix4(1.0f / (16.0f * (float)model.config().gridWidth));

	gl::setUniform(program, "uBlurWeight", 3.0f);

	for (size_t i = 0; i < model.numTiles(); ++i) {
		const SnakeTile* tilePtr = model.tilePtr(i);
		Position tilePos = model.tilePosition(tilePtr);

		// Skip non-object tiles
		if (!isObject(tilePtr)) continue;

		// Calculate base transform
		mat4 tileSpaceRot = tileSpaceRotation(tilePos.side);
		mat4 tileSpaceRotScaling = tileSpaceRot * tileScaling;
		mat4 transform = tileSpaceRotScaling;
		transform *= sfz::yRotationMatrix4(getTileAngleRad(tilePos.side, tilePtr));
		sfz::translation(transform, tilePosToVector(model, tilePos));

		// Set uniforms
		const mat4 normalMatrix = sfz::inverse(sfz::transpose(viewMatrix * transform));
		gl::setUniform(program, "uModelMatrix", transform);
		gl::setUniform(program, "uNormalMatrix", normalMatrix);

		// Render tile model
		setUniform(program, "uMaterialId", tileMaterialId(tilePtr));
		getTileModel(tilePtr, tilePos.side, model.progress(), model.isGameOver()).render();
	}
}

void renderTransparentCube(const Model& model, Program& program, const mat4& viewMatrix, vec3 camPos, size_t firstSide, size_t lastSide) noexcept
{
	Assets& assets = Assets::INSTANCE();

	const mat4 tileScaling = sfz::scalingMatrix4(1.0f / 16.0f);

	RenderOrder order = calculateRenderOrder(camPos);

	const size_t tilesPerSide = model.config().gridWidth * model.config().gridWidth;
	for (size_t side = firstSide; side <= lastSide; side++) {
		Direction currentSide = order.renderOrder[side];
		const SnakeTile* sidePtr = model.tilePtr(Position{currentSide, 0, 0});

		mat4 tileSpaceRot = tileSpaceRotation(currentSide);
		mat4 tileSpaceRotScaling = tileSpaceRot * tileScaling;

		// Calculate base transform
		mat4 transform = tileSpaceRotScaling;
		sfz::translation(transform, toVector(currentSide) * 0.5f);

		// Set uniforms
		const mat4 normalMatrix = sfz::inverse(sfz::transpose(viewMatrix * transform));
		gl::setUniform(program, "uModelMatrix", transform);
		gl::setUniform(program, "uNormalMatrix", normalMatrix);

		// Render cube tile projection
		gl::setUniform(program, "uMaterialId", MATERIAL_ID_CUBE_SIDE);
		assets.TILE_PROJECTION_MODEL.render();
	}
}

void renderSnakeProjection(const Model& model, Program& program, const mat4& viewMatrix, vec3 camPos, size_t firstSide, size_t lastSide) noexcept
{
	Assets& assets = Assets::INSTANCE();

	const mat4 tileScaling = sfz::scalingMatrix4(1.0f / (16.0f * (float)model.config().gridWidth));

	RenderOrder order = calculateRenderOrder(camPos);

	const size_t tilesPerSide = model.config().gridWidth*model.config().gridWidth;
	for (size_t side = firstSide; side <= lastSide; side++) {
		Direction currentSide = order.renderOrder[side];
		const SnakeTile* sidePtr = model.tilePtr(Position{currentSide, 0, 0});

		mat4 tileSpaceRot = tileSpaceRotation(currentSide);
		mat4 tileSpaceRotScaling = tileSpaceRot * tileScaling;

		for (size_t i = 0; i < tilesPerSide; i++) {
			const SnakeTile* tilePtr = sidePtr + i;
			Position tilePos = model.tilePosition(tilePtr);

			// Calculate base transform
			mat4 transform = tileSpaceRotScaling;
			transform *= sfz::yRotationMatrix4(getTileAngleRad(tilePos.side, tilePtr));
			sfz::translation(transform, tilePosToVector(model, tilePos));

			// Set uniforms
			const mat4 normalMatrix = sfz::inverse(sfz::transpose(viewMatrix * transform));
			gl::setUniform(program, "uModelMatrix", transform);
			gl::setUniform(program, "uNormalMatrix", normalMatrix);

			// Render tile projection
			setUniform(program, "uMaterialId", MATERIAL_ID_TILE_PROJECTION);
			auto* tileProjModelPtr = getTileProjectionModelPtr(tilePtr, tilePos.side, model.progress());
			if (tileProjModelPtr != nullptr) tileProjModelPtr->render();
		}
	}	

	// Render dead snake head projection if game over
	if (model.isGameOver()) {
		const SnakeTile* tilePtr = model.deadHeadPtr();
		Position tilePos = model.deadHeadPos();

		mat4 tileSpaceRot = tileSpaceRotation(tilePos.side);
		mat4 tileSpaceRotScaling = tileSpaceRot * tileScaling;

		// Calculate base transform
		mat4 transform = tileSpaceRotScaling;
		transform *= sfz::yRotationMatrix4(getTileAngleRad(tilePos.side, tilePtr));
		sfz::translation(transform, tilePosToVector(model, tilePos));

		// Set uniforms
		const mat4 normalMatrix = sfz::inverse(sfz::transpose(viewMatrix * transform));
		gl::setUniform(program, "uModelMatrix", transform);
		gl::setUniform(program, "uNormalMatrix", normalMatrix);

		// Render tile model
		setUniform(program, "uMaterialId", MATERIAL_ID_TILE_PROJECTION);
		getTileProjectionModelPtr(tilePtr, tilePos.side, model.progress())->render();
	}
}

} // namespace s3