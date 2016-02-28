#include "rendering/ModernRenderer.hpp"

#include <sfz/gl/OpenGL.hpp>
#include <sfz/math/Vector.hpp>
#include <sfz/util/IO.hpp>

#include "GlobalConfig.hpp"
#include "rendering/Assets.hpp"
#include "rendering/Materials.hpp"
#include "rendering/RenderingUtils.hpp"

namespace s3 {

using sfz::vec2;
using sfz::vec3;
using sfz::vec4;

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

const uint32_t GBUFFER_LINEAR_DEPTH_INDEX = 0;
const uint32_t GBUFFER_NORMAL_INDEX = 1;
const uint32_t GBUFFER_MATERIAL_INDEX = 2;
const uint32_t GBUFFER_BLUR_WEIGHTS_INDEX = 3;

static void stupidSetSpotlightUniform(const gl::Program& program, const char* name, const Spotlight& spotlight,
                                      const mat4& viewMatrix, const mat4& invViewMatrix) noexcept
{
	using std::snprintf;
	char buffer[128];
	const auto& frustum = spotlight.viewFrustum();
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "vsPos");
	gl::setUniform(program, buffer, transformPoint(viewMatrix, frustum.pos()));
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "vsDir");
	gl::setUniform(program, buffer, normalize(transformDir(viewMatrix, frustum.dir())));
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "color");
	gl::setUniform(program, buffer, spotlight.color());
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "range");
	gl::setUniform(program, buffer, frustum.far());
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "softFovRad");
	gl::setUniform(program, buffer, frustum.verticalFov() * sfz::DEG_TO_RAD());
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "sharpFovRad");
	gl::setUniform(program, buffer, spotlight.sharpFov() * sfz::DEG_TO_RAD());
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "softAngleCos");
	gl::setUniform(program, buffer, std::cos((frustum.verticalFov() / 2.0f) * sfz::DEG_TO_RAD()));
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "sharpAngleCos");
	gl::setUniform(program, buffer, std::cos((spotlight.sharpFov() / 2.0f) * sfz::DEG_TO_RAD()));
	snprintf(buffer, sizeof(buffer), "%s.%s", name, "lightMatrix");
	gl::setUniform(program, buffer, spotlight.lightMatrix(invViewMatrix));
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
		glBindFragDataLocation(shaderProgram, 0, "outFragLinearDepth");
		glBindFragDataLocation(shaderProgram, 1, "outFragNormal");
		glBindFragDataLocation(shaderProgram, 2, "outFragEmissive");
		glBindFragDataLocation(shaderProgram, 3, "outFragMaterialId");
		glBindFragDataLocation(shaderProgram, 4, "outBlurWeights");
	});

	mTransparencyProgram = Program::fromFile((sfz::basePath() + "assets/shaders/transparency.vert").c_str(),
	                                         (sfz::basePath() + "assets/shaders/transparency.frag").c_str(),
		[](uint32_t shaderProgram) {
		glBindAttribLocation(shaderProgram, 0, "inPosition");
		glBindAttribLocation(shaderProgram, 1, "inNormal");
		glBindFragDataLocation(shaderProgram, 0, "outFragColor");
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

	
	mAmbientLight = vec3(0.05f);
	mSpotlights.emplace_back(vec3{0.0f, 1.2f, 0.0f}, vec3{0.0f, -1.0f, 0.0f}, 60.0f, 50.0f, 5.0f, 0.01f, vec3{0.0f, 0.5f, 1.0f});
	mSpotlights.emplace_back(vec3{0.0f, -1.2f, 0.0f}, vec3{0.0f, 1.0f, 0.0f}, 60.0f, 50.0f, 5.0f, 0.01f, vec3{0.0f, 0.5f, 1.0f});

	mSpotlights.emplace_back(vec3{1.2f, 0.0f, 0.0f}, vec3{-1.0f, 0.0f, 0.0f}, 60.0f, 50.0f, 5.0f, 0.01f, vec3{0.0f, 0.5f, 1.0f});
	mSpotlights.emplace_back(vec3{-1.2f, 0.0f, 0.0f}, vec3{1.0f, 0.0f, 0.0f}, 60.0f, 50.0f, 5.0f, 0.01f, vec3{0.0f, 0.5f, 1.0f});

	mSpotlights.emplace_back(vec3{0.0f, 0.0f, 1.2f}, vec3{0.0f, 0.0f, -1.0f}, 60.0f, 50.0f, 5.0f, 0.01f, vec3{0.0f, 0.5f, 1.0f});
	mSpotlights.emplace_back(vec3{0.0f, 0.0f, -1.2f}, vec3{0.0f, 0.0f, 1.0f}, 60.0f, 50.0f, 5.0f, 0.01f, vec3{0.0f, 0.5f, 1.0f});



	mShadowMapHighRes = gl::createShadowMap(sfz::vec2i{512}, FBDepthFormat::F32, true, vec4{0.0f, 0.0f, 0.0f, 1.0f});
	//mShadowMapLowRes = gl::createShadowMap(sfz::vec2i{256}, FBDepthFormat::F32, true, vec4{0.0f, 0.0f, 0.0f, 1.0f});
}

// ModernRenderer: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void ModernRenderer::render(const Model& model, const Camera& cam, vec2 drawableDim, float delta) noexcept
{
	GlobalConfig& cfg = GlobalConfig::INSTANCE();
	Assets& assets = Assets::INSTANCE();

	// Ensure framebuffers are of correct size
	vec2i internalRes;
	if (cfg.gc.nativeInternalRes) {
		internalRes = vec2i{(int)drawableDim.x, (int)drawableDim.y};
	} else {
		float aspect = drawableDim.x / drawableDim.y;
		internalRes = vec2i{(int)std::round(cfg.gc.internalResolutionY * aspect), cfg.gc.internalResolutionY};
	}
	if (mGBuffer.dimensions() != internalRes) {
		vec2i blurRes{(int)(internalRes.x*cfg.gc.blurResScaling), (int)(internalRes.y*cfg.gc.blurResScaling)};
		vec2i spotlightRes{(int)(internalRes.x*cfg.gc.spotlightResScaling), (int)(internalRes.y*cfg.gc.spotlightResScaling)};
		vec2i lightShaftsRes{(int)(internalRes.x*cfg.gc.lightShaftsResScaling), (int)(internalRes.y*cfg.gc.lightShaftsResScaling)};
		
		mGBuffer = FramebufferBuilder{internalRes}
		          .addTexture(GBUFFER_LINEAR_DEPTH_INDEX, FBTextureFormat::R_F32, FBTextureFiltering::NEAREST)
		          .addTexture(GBUFFER_NORMAL_INDEX, FBTextureFormat::RGB_F32, FBTextureFiltering::NEAREST)
		          .addTexture(GBUFFER_MATERIAL_INDEX, FBTextureFormat::R_INT_U8, FBTextureFiltering::NEAREST)
		          .addTexture(GBUFFER_BLUR_WEIGHTS_INDEX, FBTextureFormat::R_F16, FBTextureFiltering::NEAREST)
		          .addDepthBuffer(FBDepthFormat::F32)
		          .build();

		mTransparencyFB = FramebufferBuilder{internalRes}
		                 .addTexture(0, FBTextureFormat::RGBA_U8, FBTextureFiltering::NEAREST)
		                 .build();
		mTransparencyFB.attachExternalDepthBuffer(mGBuffer.depthBuffer());
		
		mSpotlightShadingFB = FramebufferBuilder{spotlightRes}
		                     .addTexture(0, FBTextureFormat::RGB_U8, FBTextureFiltering::LINEAR)
		                     .addStencilBuffer()
		                     .build();

		/*mLightShaftsFB = FramebufferBuilder{lightShaftsRes}
		                .addTexture(0, FBTextureFormat::RGB_U8, FBTextureFiltering::LINEAR)
		                .addStencilBuffer()
		                .build();*/
		
		mGlobalShadingFB = FramebufferBuilder{internalRes}
		                  .addTexture(0, FBTextureFormat::RGB_U8, FBTextureFiltering::LINEAR)
		                  .build();
		
		mEmissiveFB = FramebufferBuilder{blurRes}
		             .addTexture(0, FBTextureFormat::RGB_F16, FBTextureFiltering::LINEAR)
		             .build();

		mGaussianBlur = gl::GaussianBlur{blurRes, 2, 1.0f};
		
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
		mTransparencyProgram.reload();
		mEmissiveGenProgram.reload();
		mShadowMapProgram.reload();
		mSpotlightShadingProgram.reload();
		mLightShaftsProgram.reload();
		mGlobalShadingProgram.reload();
	}

	// Update time and blur weights
	mTime += delta;
	mTime = std::fmod(mTime, 5000.0f);
	float snakeBlurWeight;
	if (model.hasTimeShiftBonus()) {
		snakeBlurWeight = 3.2f + (0.5f * (1.0f + std::sin(mTime * model.currentSpeed() * 2.5f))) * 2.0f;
	} else {
		snakeBlurWeight = 1.0 + (0.5f * (1.0f + std::sin(mTime * model.currentSpeed() * 2.5f))) * 0.75f;
	}

	// Rendering GBuffer
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	glUseProgram(mGBufferGenProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer.fbo());
	glViewport(0, 0, mGBuffer.width(), mGBuffer.height());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// View Matrix and Projection Matrix uniforms
	const auto& viewFrustum = cam.viewFrustum();
	const mat4 viewMatrix = viewFrustum.viewMatrix();
	const mat4 invViewMatrix = inverse(viewMatrix);
	const mat4 projMatrix = viewFrustum.projMatrix();
	const mat4 invProjMatrix = inverse(projMatrix);
	gl::setUniform(mGBufferGenProgram, "uProjMatrix", projMatrix);
	gl::setUniform(mGBufferGenProgram, "uViewMatrix", viewMatrix);
	gl::setUniform(mGBufferGenProgram, "uFarPlaneDist", viewFrustum.far());

	// Render things
	renderBackground(mGBufferGenProgram, viewMatrix);
	renderCube(model, mGBufferGenProgram, viewMatrix);
	renderSnake(model, mGBufferGenProgram, viewMatrix, snakeBlurWeight);
	renderObjects(model, mGBufferGenProgram, viewMatrix);

	// Rendering transparent objects
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// S(rc) = value written by fragment shader
	// D(est) = value already in framebuffer
	// O(ut) = resulting value
	//
	// Blend function for colors: Orgb = Sa*Srgb + (1-Sa)*Drgb
	// We want the final alpha value in the framebuffer (Fa) to fulfill:
	// (1-Fa) = "the amount of non-transparent background visible"
	//
	// This gives us the following blend equation for alpha values: Oa = Sa + Da - Sa*Da
	// Rewritten: Oa = Sa + Da*(1-Sa)
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

	glEnable(GL_CULL_FACE);

	glUseProgram(mTransparencyProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, mTransparencyFB.fbo());
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	gl::setUniform(mTransparencyProgram, "uProjMatrix", projMatrix);
	gl::setUniform(mTransparencyProgram, "uViewMatrix", viewMatrix);
	stupidSetUniformMaterials(mTransparencyProgram, "uMaterials");
	gl::setUniform(mTransparencyProgram, "uAmbientLight", mAmbientLight);
	
	renderSnakeProjection(model, mTransparencyProgram, viewMatrix, viewFrustum.pos());
	renderTransparentCube(model, mTransparencyProgram, viewMatrix, viewFrustum.pos());


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

	gl::setUniform(mEmissiveGenProgram, "uInvProjMatrix", invProjMatrix);
	//gl::setUniform(mEmissiveGenProgram, "uFarPlaneDist", viewFrustum.far());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.texture(GBUFFER_MATERIAL_INDEX));
	gl::setUniform(mEmissiveGenProgram, "uMaterialIdTexture", 0);	

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.texture(GBUFFER_BLUR_WEIGHTS_INDEX));
	gl::setUniform(mEmissiveGenProgram, "uBlurWeightsTexture", 1);

	stupidSetUniformMaterials(mEmissiveGenProgram, "uMaterials");

	mPostProcessQuad.render();
	
	const float blurRadiusFactor = 0.03f;
	int blurRadius = std::round(mEmissiveFB.height() * blurRadiusFactor);
	blurRadius = std::max(blurRadius, 2);
	blurRadius = ((blurRadius % 2) != 0) ? blurRadius + 1 : blurRadius;

	if (mGaussianBlur.setBlurParams(blurRadius, blurRadius*0.75f, true)) {
		/*char buffer[256];
		std::cout << "Updated gaussian blur samples (radius = " << mGaussianBlur.radius()
		          << ", sigma = " << mGaussianBlur.sigma() << "):\n";
		for (int i = 0; i < mGaussianBlur.radius(); ++i) {
			std::snprintf(buffer, sizeof(buffer), "%i: %.5f\n", i, mGaussianBlur.samples()[i]);
			std::cout << buffer;
		}*/
	}
	mGaussianBlur.apply(mEmissiveFB.fbo(), mEmissiveFB.texture(0), mEmissiveFB.dimensions());

	// Spotlights (Shadow Map + Shading + Lightshafts)
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	// Binding textures (textures may not be bound in loop)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.texture(GBUFFER_LINEAR_DEPTH_INDEX));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.texture(GBUFFER_NORMAL_INDEX));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.texture(GBUFFER_MATERIAL_INDEX));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mSpotlightShadingFB.texture(0));
	//glActiveTexture(GL_TEXTURE4);
	//glBindTexture(GL_TEXTURE_2D, mLightShaftsFB.texture(0));
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, mShadowMapHighRes.depthTexture());
	//glActiveTexture(GL_TEXTURE6);
	//glBindTexture(GL_TEXTURE_2D, mShadowMapLowRes.depthTexture());

	
	glUseProgram(mSpotlightShadingProgram.handle());
	
	// Set common Spotlight shading uniforms
	gl::setUniform(mSpotlightShadingProgram, "uInvProjMatrix", invProjMatrix);
	gl::setUniform(mSpotlightShadingProgram, "uFarPlaneDist", viewFrustum.far());
	gl::setUniform(mSpotlightShadingProgram, "uLinearDepthTexture", 0);
	gl::setUniform(mSpotlightShadingProgram, "uNormalTexture", 1);
	gl::setUniform(mSpotlightShadingProgram, "uMaterialIdTexture", 2);
	gl::setUniform(mSpotlightShadingProgram, "uShadowMap", 5);
	stupidSetUniformMaterials(mSpotlightShadingProgram, "uMaterials");
	// Clear Spotlight shading texture
	glBindFramebuffer(GL_FRAMEBUFFER, mSpotlightShadingFB.fbo());
	glViewport(0, 0, mSpotlightShadingFB.width(), mSpotlightShadingFB.height());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(mLightShaftsProgram.handle());
	
	// Set common volumetric shadows uniforms
	gl::setUniform(mLightShaftsProgram, "uInvProjMatrix", invProjMatrix);
	gl::setUniform(mLightShaftsProgram, "uFarPlaneDist", viewFrustum.far());
	gl::setUniform(mLightShaftsProgram, "uLinearDepthTexture", 0);
	gl::setUniform(mLightShaftsProgram, "uShadowMap", 6);
	
	// Clear volumetric shadows texture
	/*glBindFramebuffer(GL_FRAMEBUFFER, mLightShaftsFB.fbo());
	glViewport(0, 0, mLightShaftsFB.width(), mLightShaftsFB.height());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);*/

	

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

		renderCube(model, mShadowMapProgram, viewMatrix);
		renderSnake(model, mShadowMapProgram, viewMatrix, snakeBlurWeight);
		renderObjects(model, mShadowMapProgram, viewMatrix);
		renderOpaqueSnakeProjection(model, mShadowMapProgram, lightFrustum.viewMatrix());

		// Set low res shadow map fbo, clear it and render it
		// TODO: Might want to downscale high res shadow map for better quality low res shadow map?
		/*glBindFramebuffer(GL_FRAMEBUFFER, mShadowMapLowRes.fbo());
		glViewport(0, 0, mShadowMapLowRes.width(), mShadowMapLowRes.height());
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderOpaque(model, mShadowMapProgram, lightFrustum.viewMatrix());
		renderOpaqueSnakeProjection(model, mShadowMapProgram, lightFrustum.viewMatrix());*/

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

		/*glBindFramebuffer(GL_FRAMEBUFFER, mLightShaftsFB.fbo());
		glViewport(0, 0, mLightShaftsFB.width(), mLightShaftsFB.height());
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);

		spotlight.renderViewFrustum();*/


		glEnable(GL_CULL_FACE);
		glStencilFunc(GL_NOTEQUAL, 0, 0xFF); // Pass stencil test if not 0.
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);


		// Spotlight shading 
		glUseProgram(mSpotlightShadingProgram.handle());
		glBindFramebuffer(GL_FRAMEBUFFER, mSpotlightShadingFB.fbo());
		glViewport(0, 0, mSpotlightShadingFB.width(), mSpotlightShadingFB.height());

		stupidSetSpotlightUniform(mSpotlightShadingProgram, "uSpotlight", spotlight, viewMatrix, invViewMatrix);
		
		mPostProcessQuad.render();


		// Light shafts
		/*glUseProgram(mLightShaftsProgram.handle());
		glBindFramebuffer(GL_FRAMEBUFFER, mLightShaftsFB.fbo());
		glViewport(0, 0, mLightShaftsFB.width(), mLightShaftsFB.height());

		stupidSetSpotlightUniform(mLightShaftsProgram, "uSpotlight", spotlight, viewMatrix, invViewMatrix);

		mPostProcessQuad.render();*/
		

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

	stupidSetUniformMaterials(mGlobalShadingProgram, "uMaterials");
	gl::setUniform(mGlobalShadingProgram, "uAmbientLight", mAmbientLight);

	gl::setUniform(mGlobalShadingProgram, "uInvProjMatrix", invProjMatrix);
	gl::setUniform(mGlobalShadingProgram, "uFarPlaneDist", viewFrustum.far());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.texture(GBUFFER_LINEAR_DEPTH_INDEX));
	gl::setUniform(mGlobalShadingProgram, "uLinearDepthTexture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.texture(GBUFFER_NORMAL_INDEX));
	gl::setUniform(mGlobalShadingProgram, "uNormalTexture", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mGBuffer.texture(GBUFFER_MATERIAL_INDEX));
	gl::setUniform(mGlobalShadingProgram, "uMaterialIdTexture", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mTransparencyFB.texture(0));
	gl::setUniform(mGlobalShadingProgram, "uTransparencyTexture", 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, mSpotlightShadingFB.texture(0));
	gl::setUniform(mGlobalShadingProgram, "uSpotlightShadingTexture", 4);

	/*glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, mLightShaftsFB.texture(0));
	gl::setUniform(mGlobalShadingProgram, "uLightShaftsTexture", 5);*/

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, mEmissiveFB.texture(0));
	gl::setUniform(mGlobalShadingProgram, "uBlurredEmissiveTexture", 6);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	mPostProcessQuad.render();


	// Scale and draw resulting image to screen
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, drawableDim.x, drawableDim.y);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mScaler.changeScalingAlgorithm(static_cast<gl::ScalingAlgorithm>(cfg.gc.scalingAlgorithm));
	mScaler.scale(0, drawableDim, mGlobalShadingFB.texture(0), mGlobalShadingFB.dimensionsFloat());
}

} // namespace s3