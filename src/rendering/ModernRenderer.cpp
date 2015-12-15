#include "rendering/ModernRenderer.hpp"

#include <sfz/gl/OpenGL.hpp>
#include <sfz/math/Vector.hpp>
#include <sfz/util/IO.hpp>

#include "GlobalConfig.hpp"
#include "rendering/Assets.hpp"
#include "rendering/Materials.hpp"

namespace s3 {

using sfz::vec2;
using sfz::vec3;
using sfz::vec4;

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

const uint32_t GBUFFER_POSITION_INDEX = 0;
const uint32_t GBUFFER_NORMAL_INDEX = 1;
const uint32_t GBUFFER_MATERIAL_INDEX = 2;

static void checkGLErrorsMessage(const char* msg)
{
	if (gl::checkAllGLErrors()) std::cerr << msg << std::endl;
}

static gl::SimpleModel& getTileModel(const SnakeTile* tilePtr, Direction side, float progress,
                               bool gameOver) noexcept
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

static gl::SimpleModel* getTileProjectionModelPtr(const SnakeTile* tilePtr, Direction side, float progress) noexcept
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

static float getTileAngleRad(Direction side, const SnakeTile* tile) noexcept
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

static mat4 tileSpaceRotation(Direction side) noexcept
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

static uint32_t tileMaterialId(const SnakeTile* tilePtr) noexcept
{
	switch (tilePtr->type) {
	case s3::TileType::OBJECT:
		return MATERIAL_ID_TILE_OBJECT;

	case s3::TileType::BONUS_OBJECT:
		return MATERIAL_ID_TILE_BONUS_OBJECT;

	case s3::TileType::HEAD:
	case s3::TileType::PRE_HEAD:
	case s3::TileType::BODY:
	case s3::TileType::TAIL:
		return MATERIAL_ID_TILE_SNAKE;

	case s3::TileType::HEAD_DIGESTING:
	case s3::TileType::PRE_HEAD_DIGESTING:
	case s3::TileType::BODY_DIGESTING:
	case s3::TileType::TAIL_DIGESTING:
		return MATERIAL_ID_TILE_SNAKE_DIG;

	case s3::TileType::EMPTY:
	default:
		return MATERIAL_ID_DEFAULT;
	}
}

static uint32_t tileDecorationMaterialId(const SnakeTile* tilePtr) noexcept
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

static void renderOpaque(const Model& model, gl::Program& program, const mat4& viewMatrix) noexcept
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
		setUniform(program, "uMaterialId", tileDecorationMaterialId(tilePtr));
		assets.TILE_DECORATION_MODEL.render();

		// Skip empty tiles
		if (tilePtr->type == s3::TileType::EMPTY) continue;

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

static void renderOpaqueSnakeProjection(const Model& model, gl::Program& program, const mat4& viewMatrix) noexcept
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
		getTileProjectionModelPtr(tilePtr, tilePos.side, model.progress())->render();
	}
}

static void renderSnakeProjection(const Model& model, gl::Program& program, const mat4& viewMatrix, vec3 camPos, size_t firstSide = 0, size_t lastSide = 5) noexcept
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

static void renderTransparentCube(const Model& model, gl::Program& program, const mat4& viewMatrix, vec3 camPos, size_t firstSide = 0, size_t lastSide = 5) noexcept
{
	Assets& assets = Assets::INSTANCE();

	const mat4 tileScaling = sfz::scalingMatrix4(1.0f / 16.0f);

	RenderOrder order = calculateRenderOrder(camPos);

	const size_t tilesPerSide = model.config().gridWidth*model.config().gridWidth;
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
		setUniform(program, "uMaterialId", MATERIAL_ID_TILE_PROJECTION);
		assets.TILE_PROJECTION_MODEL.render();
	}
}

static void renderBackground(gl::Program& program, const mat4& viewMatrix) noexcept
{
	Assets& assets = Assets::INSTANCE();
	
	// Set uniforms
	const mat4 skyModelMatrix = sfz::scalingMatrix4(5.0f);
	const mat4 skyNormalMatrix = sfz::inverse(sfz::transpose(viewMatrix * skyModelMatrix));
	gl::setUniform(program, "uModelMatrix", skyModelMatrix);
	gl::setUniform(program, "uNormalMatrix", skyNormalMatrix);
	setUniform(program, "uMaterialId", MATERIAL_ID_SKY);

	assets.SKYSPHERE_MODEL.render();

	// Set uniforms
	const mat4 groundModelMatrix = sfz::translationMatrix(vec3{0.0f, -0.75, 0.0f}) * sfz::scalingMatrix4(2.5f, 1.0f, 2.5f);
	const mat4 groundNormalMatrix = sfz::inverse(sfz::transpose(viewMatrix * groundModelMatrix));
	gl::setUniform(program, "uModelMatrix", groundModelMatrix);
	gl::setUniform(program, "uNormalMatrix", groundNormalMatrix);
	setUniform(program, "uMaterialId", MATERIAL_ID_GROUND);

	//assets.GROUND_MODEL.render();
}

// ModernRenderer: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ModernRenderer::ModernRenderer() noexcept
{
	mGBufferGenProgram = Program::fromFile((sfz::basePath() + "assets/shaders/gbuffer_gen.vert").c_str(),
	                                       (sfz::basePath() + "assets/shaders/gbuffer_gen.frag").c_str(),
		[](uint32_t shaderProgram) {
		glBindAttribLocation(shaderProgram, 0, "inPosition");
		glBindAttribLocation(shaderProgram, 1, "inNormal");
		glBindFragDataLocation(shaderProgram, 0, "outFragPosition");
		glBindFragDataLocation(shaderProgram, 1, "outFragNormal");
		glBindFragDataLocation(shaderProgram, 2, "outFragEmissive");
		glBindFragDataLocation(shaderProgram, 3, "outFragMaterialId");
	});

	mEmissiveGenProgram = Program::postProcessFromFile((sfz::basePath() + "assets/shaders/emissive_gen.frag").c_str());

	mShadowMapProgram = Program::fromFile((sfz::basePath() + "assets/shaders/shadow_map.vert").c_str(),
	                                      (sfz::basePath() + "assets/shaders/shadow_map.frag").c_str(),
		[](uint32_t shaderProgram) {
		glBindAttribLocation(shaderProgram, 0, "inPosition");
		glBindAttribLocation(shaderProgram, 1, "inNormal");
		glBindFragDataLocation(shaderProgram, 0, "outFragColor");
	});

	mStencilLightProgram = Program::fromFile((sfz::basePath() + "assets/shaders/stencil_light.vert").c_str(),
	                                         (sfz::basePath() + "assets/shaders/stencil_light.frag").c_str(),
		[](uint32_t shaderProgram) {
		glBindAttribLocation(shaderProgram, 0, "inPosition");
	});

	mSpotlightShadingProgram = Program::postProcessFromFile((sfz::basePath() + "assets/shaders/spotlight_shading.frag").c_str());

	mLightShaftsProgram = Program::postProcessFromFile((sfz::basePath() + "assets/shaders/light_shafts.frag").c_str());

	mGlobalShadingProgram = Program::postProcessFromFile((sfz::basePath() + "assets/shaders/global_shading.frag").c_str());

	mSpotlights.emplace_back(vec3{0.0f, 1.2f, 0.0f}, vec3{0.0f, -1.0f, 0.0f}, 60.0f, 50.0f, 5.0f, 0.01f, vec3{0.0f, 0.5f, 1.0f});
	/*spotlightTemp.color = vec3{0.0f, 0.5f, 1.0f};
	spotlightTemp.range = 5.0f;
	spotlightTemp.fovDeg = 75.0f;
	spotlightTemp.softAngleDeg = 10.0f;
	
	spotlightTemp.pos = vec3(0.0f, 1.2f, 0.0f);
	spotlightTemp.dir = vec3(0.0f, -1.0f, 0.0f);
	mSpotlights.push_back(spotlightTemp);*/

	//spotlightTemp.pos = vec3(0.0f);

	
	/*spotlightTemp.dir = vec3(-1.0f, 0.0f, 0.0f);
	mSpotlights.push_back(spotlightTemp);
	spotlightTemp.dir = vec3(0.0f, 1.0f, 0.0f);
	mSpotlights.push_back(spotlightTemp);
	spotlightTemp.dir = vec3(0.0f, -1.0f, 0.0f);
	mSpotlights.push_back(spotlightTemp);
	spotlightTemp.dir = vec3(0.0f, 0.0f, 1.0f);
	mSpotlights.push_back(spotlightTemp);
	spotlightTemp.dir = vec3(0.0f, 0.0f, -1.0f);
	mSpotlights.push_back(spotlightTemp);*/



	mShadowMapHighRes = gl::createShadowMap(sfz::vec2i{1024}, FBDepthFormat::F32, true, vec4{0.0f, 0.0f, 0.0f, 1.0f});
	mShadowMapLowRes = gl::createShadowMap(sfz::vec2i{256}, FBDepthFormat::F16, true, vec4{0.0f, 0.0f, 0.0f, 1.0f});
}

// ModernRenderer: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void ModernRenderer::render(const Model& model, const Camera& cam, vec2 drawableDim) noexcept
{
	GlobalConfig& cfg = GlobalConfig::INSTANCE();
	Assets& assets = Assets::INSTANCE();

	// Ensure framebuffers are of correct size
	vec2i internalRes{(int)(drawableDim.x*cfg.internalResScaling), (int)(drawableDim.y*cfg.internalResScaling)};
	if (mGBuffer.dimensions() != internalRes) {
		vec2i blurRes{(int)(internalRes.x*cfg.blurResScaling), (int)(internalRes.y*cfg.blurResScaling)};
		vec2i spotlightRes{(int)(internalRes.x*cfg.spotlightResScaling), (int)(internalRes.y*cfg.spotlightResScaling)};
		vec2i lightShaftsRes{(int)(internalRes.x*cfg.lightShaftsResScaling), (int)(internalRes.y*cfg.lightShaftsResScaling)};
		
		mGBuffer = FramebufferBuilder{internalRes}
		          .addTexture(GBUFFER_POSITION_INDEX, FBTextureFormat::RGB_F32, FBTextureFiltering::NEAREST)
		          .addTexture(GBUFFER_NORMAL_INDEX, FBTextureFormat::RGB_F32, FBTextureFiltering::NEAREST)
		          .addTexture(GBUFFER_MATERIAL_INDEX, FBTextureFormat::R_INT_U8, FBTextureFiltering::NEAREST)
		          .addDepthBuffer(FBDepthFormat::F32)
		          .build();
		
		mSpotlightShadingFB = FramebufferBuilder{spotlightRes}
		                     .addTexture(0, FBTextureFormat::RGB_U8, FBTextureFiltering::LINEAR)
		                     .addStencilBuffer()
		                     .build();

		mLightShaftsFB = FramebufferBuilder{lightShaftsRes}
		                .addTexture(0, FBTextureFormat::RGB_U8, FBTextureFiltering::LINEAR)
		                .addStencilBuffer()
		                .build();
		
		mGlobalShadingFB = FramebufferBuilder{internalRes}
		                  .addTexture(0, FBTextureFormat::RGB_U8, FBTextureFiltering::LINEAR)
		                  .build();
		
		mEmissiveFB = FramebufferBuilder{blurRes}
		             .addTexture(0, FBTextureFormat::RGB_U8, FBTextureFiltering::LINEAR)
		             .build();

		mBoxBlur = gl::BoxBlur{blurRes};
		
		std::cout << "Resized framebuffers"
		          << "\nGBuffer && Global Shading resolution: " << internalRes
		          << "\nEmissive & Blur resolution: " << blurRes
		          << "\nSpotlight shading resolution: " << spotlightRes
		          << "\nLight Shafts resolution: " << lightShaftsRes
		          << "\n\n";
	}
	
	// Recompile shader programs if continuous shader reload is enabled
	if (cfg.continuousShaderReload) {
		mGBufferGenProgram.reload();
		mShadowMapProgram.reload();
		mSpotlightShadingProgram.reload();
		mLightShaftsProgram.reload();
		mGlobalShadingProgram.reload();
	}


	// Rendering GBuffer
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Disable alpha blending
	glDisable(GL_BLEND);

	// Enable culling
	glEnable(GL_CULL_FACE);

	// Binding GBufferGen program and GBuffer
	glUseProgram(mGBufferGenProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer.fbo());
	glViewport(0, 0, mGBuffer.width(), mGBuffer.height());

	// Clearing GBuffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// View Matrix and Projection Matrix uniforms
	const auto& viewFrustum = cam.viewFrustum();
	const mat4 viewMatrix = viewFrustum.viewMatrix();
	const mat4 invViewMatrix = inverse(viewMatrix);
	const mat4 projMatrix = viewFrustum.projMatrix();
	gl::setUniform(mGBufferGenProgram, "uProjMatrix", projMatrix);
	gl::setUniform(mGBufferGenProgram, "uViewMatrix", viewMatrix);

	// Render things
	renderBackground(mGBufferGenProgram, viewMatrix);
	renderOpaque(model, mGBufferGenProgram, viewMatrix);
	renderSnakeProjection(model, mGBufferGenProgram, viewMatrix, viewFrustum.pos());
	//renderTransparentCube(model, mGBufferGenProgram, viewMatrix, cam.pos(), 3, 5);


	// Emissive texture & blur
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	glUseProgram(mEmissiveGenProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, mEmissiveFB.fbo());
	glViewport(0, 0, mEmissiveFB.width(), mEmissiveFB.height());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.texture(GBUFFER_MATERIAL_INDEX));
	gl::setUniform(mEmissiveGenProgram, "uMaterialIdTexture", 0);
	stupidSetUniformMaterials(mEmissiveGenProgram, "uMaterials");

	mPostProcessQuad.render();
	
	const float blurRadiusFactor = 0.025f;
	int blurRadius = std::round(mEmissiveFB.height() * blurRadiusFactor);
	blurRadius = std::max(blurRadius, 2);
	blurRadius = ((blurRadius % 2) != 0) ? blurRadius + 1 : blurRadius;

	mBoxBlur.apply(mEmissiveFB.fbo(), mEmissiveFB.texture(0), mEmissiveFB.dimensions(), blurRadius);
	mBoxBlur.apply(mEmissiveFB.fbo(), mEmissiveFB.texture(0), mEmissiveFB.dimensions(), blurRadius);
	mBoxBlur.apply(mEmissiveFB.fbo(), mEmissiveFB.texture(0), mEmissiveFB.dimensions(), blurRadius);


	// Spotlights (Shadow Map + Shading + Lightshafts)
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	// Binding textures (textures may not be bound in loop)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.texture(GBUFFER_POSITION_INDEX));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.texture(GBUFFER_NORMAL_INDEX));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.texture(GBUFFER_MATERIAL_INDEX));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mSpotlightShadingFB.texture(0));
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, mLightShaftsFB.texture(0));
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, mShadowMapHighRes.depthTexture());
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, mShadowMapLowRes.depthTexture());

	// Clear Spotlight shading texture
	glUseProgram(mSpotlightShadingProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, mSpotlightShadingFB.fbo());
	glViewport(0, 0, mSpotlightShadingFB.width(), mSpotlightShadingFB.height());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Set common Spotlight shading uniforms
	gl::setUniform(mSpotlightShadingProgram, "uPositionTexture", 0);
	gl::setUniform(mSpotlightShadingProgram, "uNormalTexture", 1);
	gl::setUniform(mSpotlightShadingProgram, "uMaterialIdTexture", 2);
	gl::setUniform(mSpotlightShadingProgram, "uSpotlightTexture", 3);	
	gl::setUniform(mSpotlightShadingProgram, "uShadowMapHighRes", 5);
	stupidSetUniformMaterials(mSpotlightShadingProgram, "uMaterials");

	// Clear volumetric shadows texture
	glUseProgram(mLightShaftsProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, mLightShaftsFB.fbo());
	glViewport(0, 0, mLightShaftsFB.width(), mLightShaftsFB.height());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Set common volumetric shadows uniforms
	gl::setUniform(mLightShaftsProgram, "uPositionTexture", 0);
	gl::setUniform(mLightShaftsProgram, "uLightShaftsTexture", 4);
	gl::setUniform(mLightShaftsProgram, "uShadowMapLowRes", 6);

	glDisable(GL_BLEND);

	for (size_t i = 0; i < mSpotlights.size(); ++i) {
		auto& spotlight = mSpotlights[i];
		const auto& lightFrustum = spotlight.viewFrustum();

		glUseProgram(mShadowMapProgram.handle());

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		//glEnable(GL_POLYGON_OFFSET_FILL);
		//glPolygonOffset(5.0f, 25.0f);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		gl::setUniform(mShadowMapProgram, "uViewProjMatrix", lightFrustum.projMatrix() * lightFrustum.viewMatrix());

		// Set high res shadow map fbo, clear it and render it
		glBindFramebuffer(GL_FRAMEBUFFER, mShadowMapHighRes.fbo());
		glViewport(0, 0, mShadowMapHighRes.width(), mShadowMapHighRes.height());
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderOpaque(model, mShadowMapProgram, lightFrustum.viewMatrix());
		renderOpaqueSnakeProjection(model, mShadowMapProgram, lightFrustum.viewMatrix());

		// Set low res shadow map fbo, clear it and render it
		// TODO: Might want to downscale high res shadow map for better quality low res shadow map?
		glBindFramebuffer(GL_FRAMEBUFFER, mShadowMapLowRes.fbo());
		glViewport(0, 0, mShadowMapLowRes.width(), mShadowMapLowRes.height());
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderOpaque(model, mShadowMapProgram, lightFrustum.viewMatrix());
		renderOpaqueSnakeProjection(model, mShadowMapProgram, lightFrustum.viewMatrix());

		//glDisable(GL_POLYGON_OFFSET_FILL);
		glCullFace(GL_BACK);
		
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);


		// Spotlight & light shafts stencil buffer
		glUseProgram(mStencilLightProgram.handle());
		
		glDisable(GL_CULL_FACE);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glStencilOp(GL_INCR, GL_INCR, GL_INCR);
		
		gl::setUniform(mStencilLightProgram, "uViewProjMatrix", projMatrix * viewMatrix);
		gl::setUniform(mStencilLightProgram, "uModelMatrix", spotlight.viewFrustumTransform());

		glBindFramebuffer(GL_FRAMEBUFFER, mSpotlightShadingFB.fbo());
		glViewport(0, 0, mSpotlightShadingFB.width(), mSpotlightShadingFB.height());
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);

		spotlight.renderViewFrustum();

		glBindFramebuffer(GL_FRAMEBUFFER, mLightShaftsFB.fbo());
		glViewport(0, 0, mLightShaftsFB.width(), mLightShaftsFB.height());
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);

		spotlight.renderViewFrustum();

		glEnable(GL_CULL_FACE);
		glStencilFunc(GL_NOTEQUAL, 0, 0xFF); // Pass stencil test if not 0.
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);


		// Spotlight shading 
		glUseProgram(mSpotlightShadingProgram.handle());
		glBindFramebuffer(GL_FRAMEBUFFER, mSpotlightShadingFB.fbo());
		glViewport(0, 0, mSpotlightShadingFB.width(), mSpotlightShadingFB.height());

		stupidSetSpotLightUniform(mSpotlightShadingProgram, "uSpotlight", spotlight, viewMatrix, invViewMatrix);
		
		mPostProcessQuad.render();


		// Light shafts
		glUseProgram(mLightShaftsProgram.handle());
		glBindFramebuffer(GL_FRAMEBUFFER, mLightShaftsFB.fbo());
		glViewport(0, 0, mLightShaftsFB.width(), mLightShaftsFB.height());

		stupidSetSpotLightUniform(mLightShaftsProgram, "uSpotlight", spotlight, viewMatrix, invViewMatrix);

		mPostProcessQuad.render();
		

		glDisable(GL_STENCIL_TEST);
	}


	// Global shading
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	glUseProgram(mGlobalShadingProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, mGlobalShadingFB.fbo());
	glViewport(0, 0, mGlobalShadingFB.width(), mGlobalShadingFB.height());

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	stupidSetUniformMaterials(mGlobalShadingProgram, "uMaterials");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.texture(GBUFFER_POSITION_INDEX));
	gl::setUniform(mGlobalShadingProgram, "uPositionTexture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.texture(GBUFFER_NORMAL_INDEX));
	gl::setUniform(mGlobalShadingProgram, "uNormalTexture", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.texture(GBUFFER_MATERIAL_INDEX));
	gl::setUniform(mGlobalShadingProgram, "uMaterialIdTexture", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mSpotlightShadingFB.texture(0));
	gl::setUniform(mGlobalShadingProgram, "uSpotlightShadingTexture", 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, mLightShaftsFB.texture(0));
	gl::setUniform(mGlobalShadingProgram, "uLightShaftsTexture", 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, mEmissiveFB.texture(0));
	gl::setUniform(mGlobalShadingProgram, "uBlurredEmissiveTexture", 5);

	mPostProcessQuad.render();


	// Scale and draw resulting image to screen
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, drawableDim.x, drawableDim.y);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mScaler.changeScalingAlgorithm(static_cast<gl::ScalingAlgorithm>(cfg.scalingAlgorithm));
	mScaler.scale(0, drawableDim, mGlobalShadingFB.texture(0), mGlobalShadingFB.dimensionsFloat());
}

} // namespace s3