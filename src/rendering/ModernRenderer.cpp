#include "rendering/ModernRenderer.hpp"

#include <sfz/gl/OpenGL.hpp>
#include <sfz/math/Vector.hpp>
#include <sfz/util/IO.hpp>

#include "GlobalConfig.hpp"
#include "rendering/Assets.hpp"

namespace s3 {

using sfz::vec2;
using sfz::vec3;
using sfz::vec4;

// Static functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static void checkGLErrorsMessage(const char* msg)
{
	if (gl::checkAllGLErrors()) std::cerr << msg << std::endl;
}

static gl::Program compileStandardShaderProgram() noexcept
{
	return gl::Program::fromFile((sfz::basePath() + "assets/shaders/shader.vert").c_str(),
	                             (sfz::basePath() + "assets/shaders/shader.frag").c_str(),
	                             [](uint32_t shaderProgram) {
		glBindAttribLocation(shaderProgram, 0, "inPosition");
		glBindAttribLocation(shaderProgram, 1, "inNormal");
		glBindAttribLocation(shaderProgram, 2, "inUV"); // Not available for snakium models
		glBindAttribLocation(shaderProgram, 3, "inMaterialID");
		glBindFragDataLocation(shaderProgram, 0, "outFragColor");
	});
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

static vec4 tileColor(const SnakeTile* tilePtr) noexcept
{
	switch (tilePtr->type) {

	case s3::TileType::OBJECT:
		return vec4(0.0f, 1.0f, 1.0f, 1.0f);

	case s3::TileType::BONUS_OBJECT:
		return vec4{1.0f, 0.0f, 0.85f, 1.0f};

	case s3::TileType::HEAD:
	case s3::TileType::PRE_HEAD:
	case s3::TileType::BODY:
	case s3::TileType::TAIL:
		return vec4{0.0f, 1.0f, 0.25f, 1.0f};

	case s3::TileType::HEAD_DIGESTING:
	case s3::TileType::PRE_HEAD_DIGESTING:
	case s3::TileType::BODY_DIGESTING:
	case s3::TileType::TAIL_DIGESTING:
		return vec4{0.0f, 1.0f, 0.25f, 1.0f};

	case s3::TileType::EMPTY:
	default:
		return vec4{1.0f, 1.0f, 1.0f, 1.0f};
	}
}

static vec4 tileDecorationColor(const SnakeTile* tilePtr) noexcept
{
	const vec4 TILE_DECORATION_COLOR{0.25f, 0.5f, 0.5f, 1.0f};
	const vec4 TILE_DECORATION_OCCUPIED_COLOR{0.0f, 1.0f, 0.25f, 1.0f};

	switch (tilePtr->type) {
	case TileType::EMPTY:
	case TileType::OBJECT:
	case TileType::BONUS_OBJECT:
		return TILE_DECORATION_COLOR;
	default:
		return TILE_DECORATION_OCCUPIED_COLOR;
	}
}

static vec4 tileCubeProjectionColor(const SnakeTile* tilePtr) noexcept
{
	const vec4 TILE_CUBE_PROJECTION_COLOR{0.25f, 0.25f, 0.25f, 0.6f};
	return TILE_CUBE_PROJECTION_COLOR;
	/*const vec4 TILE_CUBE_PROJECTION_OCCUPIED_COLOR{0.25f, 0.275f, 0.25f, 0.6f};

	switch (tilePtr->type) {
	case TileType::EMPTY:
	case TileType::OBJECT:
	case TileType::BONUS_OBJECT:
		return TILE_CUBE_PROJECTION_COLOR;
	default:
		return TILE_CUBE_PROJECTION_OCCUPIED_COLOR;
	}*/
}

static void renderOpaque(const Model& model, gl::Program& program, const mat4& viewMatrix) noexcept
{
	Assets& assets = Assets::INSTANCE();
	const vec4 TILE_DIVE_ASCEND_COLOR{0.5f, 0.0f, 0.75f, 1.0f};

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
		gl::setUniform(program, "uColor", tileDecorationColor(tilePtr));
		assets.TILE_DECORATION_MODEL.render();

		// Skip empty tiles
		if (tilePtr->type == s3::TileType::EMPTY) continue;

		// Render dive & ascend models
		if (isDive(tilePos.side, tilePtr->to)) {
			gl::setUniform(program, "uColor", TILE_DIVE_ASCEND_COLOR);
			assets.DIVE_MODEL.render();
		} else if (isAscend(tilePos.side, tilePtr->from) &&
			tilePtr->type != TileType::TAIL && tilePtr->type != TileType::TAIL_DIGESTING) {
			gl::setUniform(program, "uColor", TILE_DIVE_ASCEND_COLOR);
			assets.ASCEND_MODEL.render();
		}

		// Render tile model
		gl::setUniform(program, "uColor", tileColor(tilePtr));
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
			gl::setUniform(program, "uColor", TILE_DIVE_ASCEND_COLOR);
			assets.DIVE_MODEL.render();
		} else if (isAscend(tilePos.side, tilePtr->from) &&
			tilePtr->type != TileType::TAIL && tilePtr->type != TileType::TAIL_DIGESTING) {
			gl::setUniform(program, "uColor", TILE_DIVE_ASCEND_COLOR);
			assets.ASCEND_MODEL.render();
		}

		// Render tile model
		gl::setUniform(program, "uColor", tileColor(tilePtr));
		getTileModel(tilePtr, tilePos.side, model.progress(), model.isGameOver()).render();
	}
}

static void renderTransparent(const Model& model, gl::Program& program, const Camera& cam) noexcept
{
	Assets& assets = Assets::INSTANCE();
	const vec4 TILE_PROJECTION_COLOR{0.5f, 0.5f, 0.5f, 0.75f};

	const mat4 tileScaling = sfz::scalingMatrix4(1.0f / (16.0f * (float)model.config().gridWidth));

	RenderOrder order = calculateRenderOrder(cam.pos());

	const size_t tilesPerSide = model.config().gridWidth*model.config().gridWidth;
	for (size_t side = 0; side < 6; side++) {
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
			const mat4 normalMatrix = sfz::inverse(sfz::transpose(cam.viewMatrix() * transform));
			gl::setUniform(program, "uModelMatrix", transform);
			gl::setUniform(program, "uNormalMatrix", normalMatrix);

			if (order.renderTileFaceFirst[side]) {

				// Render cube tile projection
				gl::setUniform(program, "uColor", tileCubeProjectionColor(tilePtr));
				assets.TILE_PROJECTION_MODEL.render();

				// Render tile projection
				gl::setUniform(program, "uColor", TILE_PROJECTION_COLOR);
				auto* tileProjModelPtr = getTileProjectionModelPtr(tilePtr, tilePos.side, model.progress());
				if (tileProjModelPtr != nullptr) tileProjModelPtr->render();

			} else {

				// Render tile projection
				gl::setUniform(program, "uColor", TILE_PROJECTION_COLOR);
				auto* tileProjModelPtr = getTileProjectionModelPtr(tilePtr, tilePos.side, model.progress());
				if (tileProjModelPtr != nullptr) tileProjModelPtr->render();

				// Render cube tile projection
				gl::setUniform(program, "uColor", tileCubeProjectionColor(tilePtr));
				assets.TILE_PROJECTION_MODEL.render();
			}
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
		const mat4 normalMatrix = sfz::inverse(sfz::transpose(cam.viewMatrix() * transform));
		gl::setUniform(program, "uModelMatrix", transform);
		gl::setUniform(program, "uNormalMatrix", normalMatrix);

		// Render tile model
		gl::setUniform(program, "uColor", TILE_PROJECTION_COLOR);
		getTileProjectionModelPtr(tilePtr, tilePos.side, model.progress())->render();
	}
}

static void renderBackground(gl::Program& program, const mat4& viewMatrix) noexcept
{
	const vec4 SKY_COLOR{0.2f, 0.2f, 0.2f, 1.0f};
	const vec4 GROUND_COLOR{0.5f, 0.5f, 0.5f, 1.0f};

	Assets& assets = Assets::INSTANCE();
	
	// Set uniforms
	const mat4 skyModelMatrix = sfz::scalingMatrix4(5.0f);
	const mat4 skyNormalMatrix = sfz::inverse(sfz::transpose(viewMatrix * skyModelMatrix));
	gl::setUniform(program, "uModelMatrix", skyModelMatrix);
	gl::setUniform(program, "uNormalMatrix", skyNormalMatrix);
	gl::setUniform(program, "uColor", SKY_COLOR);

	assets.SKYSPHERE_MODEL.render();

	// Set uniforms
	const mat4 groundModelMatrix = sfz::translationMatrix(vec3{0.0f, -0.75, 0.0f}) * sfz::scalingMatrix4(2.5f, 1.0f, 2.5f);
	const mat4 groundNormalMatrix = sfz::inverse(sfz::transpose(viewMatrix * groundModelMatrix));
	gl::setUniform(program, "uModelMatrix", groundModelMatrix);
	gl::setUniform(program, "uNormalMatrix", groundNormalMatrix);
	gl::setUniform(program, "uColor", GROUND_COLOR);

	assets.GROUND_MODEL.render();
}

// ModernRenderer: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ModernRenderer::ModernRenderer() noexcept
:
	mProgram{s3::compileStandardShaderProgram()}
{
	mSpotLight.pos = vec3(0.0f, 2.0f, 0.0f);
	mSpotLight.dir = vec3(0.0f, -1.0f, 0.0f);
	mSpotLight.angle = 90.0f;
	mSpotLight.reach = 5.0f;
}

// ModernRenderer: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void ModernRenderer::render(const Model& model, const Camera& cam, vec2 drawableDim) noexcept
{
	GlobalConfig& cfg = GlobalConfig::INSTANCE();
	Assets& assets = Assets::INSTANCE();

	// Ensure framebuffers are of correct size
	vec2i internalRes{(int)(drawableDim.x*cfg.internalResScaling), (int)(drawableDim.y*cfg.internalResScaling)};
	if (mExternalFB.dimensionsInt() != internalRes) {
		mExternalFB = ExternalFB{internalRes};
		std::cout << "Resized xfb, new size: " << mExternalFB.dimensionsInt() << std::endl;
	}
	
	// Recompile shader programs if continuous shader reload is enabled
	if (cfg.continuousShaderReload) {
		mProgram.reload();
	}

	// Enable depth testing
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable culling
	glEnable(GL_CULL_FACE);

	// Binding external framebuffer
	glUseProgram(mProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, mExternalFB.fbo());
	glViewport(0, 0, mExternalFB.dimensionsInt().x, mExternalFB.dimensionsInt().y);

	// Clearing screen
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// View Matrix and Projection Matrix uniforms
	const mat4 viewMatrix = cam.viewMatrix();
	//const mat4 invViewMatrix = sfz::inverse(viewMatrix);
	gl::setUniform(mProgram, "uProjMatrix", cam.projMatrix());
	gl::setUniform(mProgram, "uViewMatrix", viewMatrix);
	
	// SpotLight uniforms (wip)
	gl::setUniform(mProgram, "uSpotLightPos", sfz::transformPoint(viewMatrix, mSpotLight.pos));
	gl::setUniform(mProgram, "uSpotLightDir", sfz::transformDir(viewMatrix, mSpotLight.dir));
	gl::setUniform(mProgram, "uSpotLightReach", mSpotLight.reach);
	gl::setUniform(mProgram, "uSpotLightAngle", mSpotLight.angle);

	// Render background
	renderBackground(mProgram, viewMatrix);

	// Render opaque objects
	renderOpaque(model, mProgram, viewMatrix);

	// Render transparent objects
	renderTransparent(model, mProgram, cam);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, drawableDim.x, drawableDim.y);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mScaler.changeScalingAlgorithm(static_cast<gl::ScalingAlgorithm>(cfg.scalingAlgorithm));
	mScaler.scale(0, drawableDim, mExternalFB.colorTexture(), mExternalFB.dimensions());
}

} // namespace s3