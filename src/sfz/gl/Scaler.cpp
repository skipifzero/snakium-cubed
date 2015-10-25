#include "sfz/gl/Scaler.hpp"

#include <algorithm>

#include "sfz/gl/OpenGL.hpp"
#include "sfz/gl/PostProcessQuad.hpp"
#include "sfz/gl/Program.hpp"

namespace gl {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const char* SIMPLE_1_SAMPLE_SHADER_SRC = R"(
	#version 330

	// Input
	in vec2 uvCoord;

	// Uniforms
	uniform sampler2D uSrcTex;
	uniform vec2 uDstDimensions;
	uniform vec2 uSrcDimensions;

	// Output
	out vec4 outFragColor;

	void main()
	{
		outFragColor = texture(uSrcTex, uvCoord);
	}
)";

static const char* GRID_4_SAMPLES_SHADER_SRC = R"(
	#version 330

	// Input
	in vec2 uvCoord;

	// Uniforms
	uniform sampler2D uSrcTex;
	uniform vec2 uDstDimensions;
	uniform vec2 uSrcDimensions;

	// Output
	out vec4 outFragColor;

	void main()
	{
		vec2 fragSize = vec2(1.0) / uDstDimensions;
		vec2 quartFragSize = fragSize / 4.0;

		vec2 bottomLeftUV = uvCoord - quartFragSize;
		vec2 bottomRightUV = uvCoord + vec2(quartFragSize.x, -quartFragSize.y);
		vec2 topLeftUV = uvCoord + vec2(-quartFragSize.x, quartFragSize.y);
		vec2 topRightUV = uvCoord + quartFragSize;

		outFragColor = (texture(uSrcTex, bottomLeftUV) + texture(uSrcTex, bottomRightUV)
		             + texture(uSrcTex, topLeftUV) + texture(uSrcTex, topRightUV))/4.0f;
	}
)";

static const char* BICUBIC_SHADER_SRC = R"(
	#version 330

	// Input
	in vec2 uvCoord;

	// Uniforms
	uniform sampler2D uSrcTex;
	uniform vec2 uDstDimensions;
	uniform vec2 uSrcDimensions;

	// Output
	out vec4 outFragColor;

	float BSpline(float f)
	{
		f = abs(f);
		if (f <= 1.0) {
			return (2.0 / 3.0) + (0.5 * pow(f,3)) - pow(f,2);
		}
		else if (f <= 2.0) {
			return (1.0 / 6.0) * pow((2.0 - f), 3);
		}
		return 1.0;
	}

	void main()
	{
		// Stupid 16 sample bicubic based on: 
		// http://www.codeproject.com/Articles/236394/Bi-Cubic-and-Bi-Linear-Interpolation-with-GLSL#GLSLBiCubic
		// The code quality is a bit lacking, so I'm not entirely sure the given algorithm is actually correct.

		vec2 texelSize = vec2(1.0) / uSrcDimensions;
		vec2 coordFract = fract(uvCoord * uSrcDimensions); // The fractional value [0, 1] between pixels on src image

		vec4 sumNumer = vec4(0);
		vec4 sumDenom = vec4(0);

		for (int y = -1; y <= 2; ++y) {
			for (int x = -1; x <= 2; ++x) {
				vec2 offs = vec2(float(x), float(y));
				vec4 sample = texture(uSrcTex, uvCoord + offs*texelSize);
				
				float coeef1 = BSpline(offs.x - coordFract.x);
				//float coeef2 = BSpline(-offs.y - coordFract.y);
				float coeef2 = BSpline(offs.y - coordFract.y);

				sumNumer += (sample * coeef1 * coeef2);
				sumDenom += (coeef1 * coeef2);
			}
		}

		outFragColor = sumNumer / sumDenom;
	}
)";

// Scaler: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Scaler::Scaler(ScalingAlgorithm scalingAlgo) noexcept
{
	this->mScalingAlgorithm = (ScalingAlgorithm)(((int32_t)scalingAlgo)+1);
	this->changeScalingAlgorithm(scalingAlgo);
}

Scaler::Scaler(Scaler&& other) noexcept
{
	std::swap(this->mScalingAlgorithm, other.mScalingAlgorithm);
	std::swap(this->mProgram, other.mProgram);
}

Scaler& Scaler::operator= (Scaler&& other) noexcept
{
	std::swap(this->mScalingAlgorithm, other.mScalingAlgorithm);
	std::swap(this->mProgram, other.mProgram);
	return *this;
}

Scaler::~Scaler() noexcept
{
	glDeleteSamplers(1, &mSamplerObject);
}

// Scaler: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Scaler::scale(uint32_t dstFBO, vec2 dstDimensions, uint32_t srcTex, vec2 srcDimensions) noexcept
{
	scale(dstFBO, AABB2D{dstDimensions/2.0f, dstDimensions}, srcTex, srcDimensions);
}

void Scaler::scale(uint32_t dstFBO, const AABB2D& dstViewport, uint32_t srcTex, vec2 srcDimensions) noexcept
{
	// Bind shader, framebuffer and viewport
	glUseProgram(mProgram.handle());
	glBindFramebuffer(GL_FRAMEBUFFER, dstFBO);
	glViewport(dstViewport.min.x, dstViewport.min.y, dstViewport.width(), dstViewport.height());

	// Bind src texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTex);
	gl::setUniform(mProgram, "uSrcTex", 0);
	glBindSampler(0, mSamplerObject);

	gl::setUniform(mProgram, "uDstDimensions", dstViewport.dimensions());
	gl::setUniform(mProgram, "uSrcDimensions", srcDimensions);

	mQuad.render();

	// Cleanup
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(0, 0);
}

void Scaler::changeScalingAlgorithm(ScalingAlgorithm newAlgo) noexcept
{
	if (mScalingAlgorithm == newAlgo) return;

	mScalingAlgorithm = newAlgo;
	glDeleteSamplers(1, &mSamplerObject);
	glGenSamplers(1, &mSamplerObject);

	switch (mScalingAlgorithm) {
	case ScalingAlgorithm::NEAREST:
		mProgram = gl::Program::postProcessFromSource(SIMPLE_1_SAMPLE_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case ScalingAlgorithm::BILINEAR:
		mProgram = gl::Program::postProcessFromSource(SIMPLE_1_SAMPLE_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case ScalingAlgorithm::GRID_4_NEAREST:
		mProgram = gl::Program::postProcessFromSource(GRID_4_SAMPLES_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case ScalingAlgorithm::GRID_4_BILINEAR:
		mProgram = gl::Program::postProcessFromSource(GRID_4_SAMPLES_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case ScalingAlgorithm::BICUBIC:
		mProgram = gl::Program::postProcessFromSource(BICUBIC_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // TODO: CHANGE TO LINEAR WHEN PROPER BICUBIC IMPLEMENTATION
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	default:
		sfz_assert_release_m(false, "Invalid scaling algorithm.");
	}
}

} // namespace sfz