#include "sfz/gl/Scaler.hpp"

#include <algorithm>

#include "sfz/gl/OpenGL.hpp"
#include "sfz/gl/PostProcessQuad.hpp"
#include "sfz/gl/Program.hpp"

namespace gl {

// Statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const char* SINGLE_SAMPLE_SHADER_SRC = R"(
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

static const char* GRID_2X2_SHADER_SRC = R"(
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

static const char* GRID_4X4_SHADER_SRC = R"(
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
		vec2 fragSizeDiv4 = fragSize / 4.0;
		vec2 fragSizeDiv8 = fragSize / 8.0;

		// Calculate 8x8 grid coordinates
		vec2 coord00 = uvCoord - 3.0*fragSizeDiv8;
		vec2 coord10 = coord00 + vec2(fragSizeDiv4.x, 0);
		vec2 coord20 = coord10 + vec2(fragSizeDiv4.x, 0);
		vec2 coord30 = coord20 + vec2(fragSizeDiv4.x, 0);

		vec2 coord01 = coord00 + vec2(0, fragSizeDiv4.y);
		vec2 coord11 = coord10 + vec2(0, fragSizeDiv4.y);
		vec2 coord21 = coord20 + vec2(0, fragSizeDiv4.y);
		vec2 coord31 = coord30 + vec2(0, fragSizeDiv4.y);

		vec2 coord02 = coord01 + vec2(0, fragSizeDiv4.y);
		vec2 coord12 = coord11 + vec2(0, fragSizeDiv4.y);
		vec2 coord22 = coord21 + vec2(0, fragSizeDiv4.y);
		vec2 coord32 = coord31 + vec2(0, fragSizeDiv4.y);

		vec2 coord03 = coord02 + vec2(0, fragSizeDiv4.y);
		vec2 coord13 = coord12 + vec2(0, fragSizeDiv4.y);
		vec2 coord23 = coord22 + vec2(0, fragSizeDiv4.y);
		vec2 coord33 = coord32 + vec2(0, fragSizeDiv4.y);

		outFragColor = vec4((
		             texture(uSrcTex, coord00).rgb
		             + texture(uSrcTex, coord10).rgb
		             + texture(uSrcTex, coord20).rgb
		             + texture(uSrcTex, coord30).rgb

		             + texture(uSrcTex, coord01).rgb
		             + texture(uSrcTex, coord11).rgb
		             + texture(uSrcTex, coord21).rgb
		             + texture(uSrcTex, coord31).rgb

		             + texture(uSrcTex, coord02).rgb
		             + texture(uSrcTex, coord12).rgb
		             + texture(uSrcTex, coord22).rgb
		             + texture(uSrcTex, coord32).rgb

		             + texture(uSrcTex, coord03).rgb
		             + texture(uSrcTex, coord13).rgb
		             + texture(uSrcTex, coord23).rgb
		             + texture(uSrcTex, coord33).rgb) / 16.0, 1.0);
	}
)";

static const char* BICUBIC_BSPLINE_SHADER_SRC = R"(
	#version 330

	// Input
	in vec2 uvCoord;

	// Uniforms
	uniform sampler2D uSrcTex;
	uniform vec2 uDstDimensions;
	uniform vec2 uSrcDimensions;

	// Output
	out vec4 outFragColor;

	/*// Reference implementation of the BSpline weights
	// dist is the distance to the texel to be sampled from the sampling point
	vec2 BSplineWeight(vec2 dist)
	{
		dist = abs(dist);

		vec2 res = vec2(0);
		if (dist.x <= 1.0) {
			res.x = 4.0 + (3.0 * pow(dist.x, 3)) - (6.0 * pow(dist.x, 2));
		} else if (dist.x <= 2.0) {
			res.x = pow(2.0 - dist.x, 3)
		}
		if (dist.y <= 1.0) {
			res.y = 4.0 + (3.0 * pow(dist.y, 3)) - (6.0 * pow(dist.y, 2));
		} else if (dist.y <= 2.0) {
			res.y = pow(2.0 - dist.y, 3)
		}
		
		return (1.0 / 6.0) * res;
	}*/

	// See: http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter20.html
	// and: http://vec3.ca/bicubic-filtering-in-fewer-taps/
	void main()
	{
		vec2 pixSize = vec2(1.0) / uSrcDimensions;
		vec2 pixCoord = uvCoord * uSrcDimensions;
		vec2 texCenter = floor(pixCoord - vec2(0.5)) + vec2(0.5); // Coordinate to nearest texel center

		// Offset from nearest texel center to pixCoord
		vec2 fractOffs = pixCoord - texCenter;
		vec2 fractOffsPow2 = fractOffs * fractOffs;
		vec2 fractOffsPow3 = fractOffsPow2 * fractOffs;

		// Note, below each index is off by one. I.e, index 0 is actually -1,
		// 1 is actually 0, and 2 is 1, etc.

		// BSpline filter weights
		// See the reference implementation to see how these are calculated in the general case
		// -1: abs(dist) = fractOffs + 1 => case 2
		// 0: abs(dist) = fractOffs => case 1
		// 1: abs(dist) = 1 - fractOffs => case 1
		// 2: abs(dist) = 2 - fractOffs => case 2

		vec2 oneMinFractOffs = vec2(1.0) - fractOffs;
		vec2 oneMinFractOffsPow2 = oneMinFractOffs * oneMinFractOffs;
		vec2 oneMinFractOffsPow3 = oneMinFractOffsPow2 * oneMinFractOffs;
		vec2 w0 = (1.0/6.0) * oneMinFractOffsPow3;
		vec2 w1 = (1.0/6.0) * (vec2(4.0) + (3.0*fractOffsPow3) - (6.0*fractOffsPow2));
		vec2 w2 = (1.0/6.0) * (vec2(4.0) + (3.0*oneMinFractOffsPow3) - (6.0*oneMinFractOffsPow2));
		vec2 w3 = (1.0/6.0) * fractOffsPow3;

		/* // Optimized weight calculations, not 100% sure if correct
		vec2 w0 = fractOffsPow2 - (0.5 * (fractOffsPow3 + fractOffs));
		vec2 w1 = 1.5*fractOffsPow3 - 2.5*fractOffsPow2 + vec2(1.0);
		vec2 w3 = 0.5 * (fractOffsPow3 - fractOffsPow2);
		vec2 w2 = vec2(1.0) - w0 - w1 - w3;*/
		
		// Calculate sample coordinates
		vec2 f0 = w1 / (w0 + w1);
		vec2 f1 = w3 / (w2 + w3);

		vec2 cornerCoord0 = (texCenter - vec2(1.0) + f0) * pixSize;
		vec2 cornerCoord1 = (texCenter + vec2(1.0) + f1) * pixSize;

		vec2 coord00 = cornerCoord0;
		vec2 coord10 = vec2(cornerCoord1.x, cornerCoord0.y);
		vec2 coord01 = vec2(cornerCoord0.x, cornerCoord1.y);
		vec2 coord11 = cornerCoord1;

		// Take samples
		vec4 sample00 = texture(uSrcTex, coord00);
		vec4 sample10 = texture(uSrcTex, coord10);
		vec4 sample01 = texture(uSrcTex, coord01);
		vec4 sample11 = texture(uSrcTex, coord11);

		// Calculate scale used to scale samples
		vec2 scale0 = w0 + w1;
		vec2 scale1 = w2 + w3;

		outFragColor = (sample00 * scale0.x + sample10 * scale1.x) * scale0.y
		             + (sample01 * scale0.x + sample11 * scale1.x) * scale1.y;
	}
)";

static const char* LANCZOS_2_SHADER_SRC = R"(
	#version 330

	// Input
	in vec2 uvCoord;

	// Uniforms
	uniform sampler2D uSrcTex;
	uniform vec2 uDstDimensions;
	uniform vec2 uSrcDimensions;

	// Output
	out vec4 outFragColor;

	// Constants
	const float A = 2.0;
	const int N = int(A);
	const float PI = 3.14159265358979323846264338327950;
	const float A_DIV_PI2 = A / (PI * PI);
	const float A_INV = 1.0 / A;

	float L(float x)
	{
		if (x == 0) return 1.0;
		float PIx = PI * x;
		return A_DIV_PI2 * ((sin(PIx) * sin(PIx * A_INV)) / (x * x));
	}

	void main()
	{
		vec2 texelSize = vec2(1.0 / uSrcDimensions);
		vec2 pixCoord = uvCoord * uSrcDimensions;
		vec2 texCenter = floor(pixCoord) + vec2(0.5); // Coordinate to nearest texel center	
		vec2 texCenterOffs = pixCoord - texCenter;
		
		vec3 sum = vec3(0.0);
		float totalWeight = 0.0;
		for (int x = -N + 1; x <= N; ++x) {
			float xf = float(x);
			float xWeight = L(xf - texCenterOffs.x);
			
			for (int y = -N + 1; y <= N; ++y) {
				float yf = float(y);
				float yWeight = L(yf - texCenterOffs.y);
				vec2 coord = (texCenter + vec2(xf, yf)) * texelSize;
				sum += (xWeight * yWeight * texture(uSrcTex, coord).rgb);

				totalWeight += (xWeight * yWeight);
			}
		}
		
		outFragColor = vec4(sum / totalWeight, 1.0);
	}
)";

static const char* LANCZOS_3_SHADER_SRC = R"(
	#version 330

	// Input
	in vec2 uvCoord;

	// Uniforms
	uniform sampler2D uSrcTex;
	uniform vec2 uDstDimensions;
	uniform vec2 uSrcDimensions;

	// Output
	out vec4 outFragColor;

	// Constants
	const float A = 3.0;
	const int N = int(A);
	const float PI = 3.14159265358979323846264338327950;
	const float A_DIV_PI2 = A / (PI * PI);
	const float A_INV = 1.0 / A;

	float L(float x)
	{
		if (x == 0) return 1.0;
		float PIx = PI * x;
		return A_DIV_PI2 * ((sin(PIx) * sin(PIx * A_INV)) / (x * x));
	}

	void main()
	{
		vec2 texelSize = vec2(1.0 / uSrcDimensions);
		vec2 pixCoord = uvCoord * uSrcDimensions;
		vec2 texCenter = floor(pixCoord) + vec2(0.5); // Coordinate to nearest texel center	
		vec2 texCenterOffs = pixCoord - texCenter;
		
		vec3 sum = vec3(0.0);
		float totalWeight = 0.0;
		for (int x = -N + 1; x <= N; ++x) {
			float xf = float(x);
			float xWeight = L(xf - texCenterOffs.x);
			
			for (int y = -N + 1; y <= N; ++y) {
				float yf = float(y);
				float yWeight = L(yf - texCenterOffs.y);
				vec2 coord = (texCenter + vec2(xf, yf)) * texelSize;
				sum += (xWeight * yWeight * texture(uSrcTex, coord).rgb);

				totalWeight += (xWeight * yWeight);
			}
		}
		
		outFragColor = vec4(sum / totalWeight, 1.0);
	}
)";

static const char* LANCZOS_VARIABLE_SHADER_SRC = R"(
	#version 330

	// Input
	in vec2 uvCoord;

	// Uniforms
	uniform sampler2D uSrcTex;
	uniform vec2 uDstDimensions;
	uniform vec2 uSrcDimensions;

	// Output
	out vec4 outFragColor;

	// Constants
	const float A = 2.0;
	const int N = int(A);
	const float PI = 3.14159265358979323846264338327950;
	const float A_DIV_PI2 = A / (PI * PI);
	const float A_INV = 1.0 / A;

	float L(float x)
	{
		if (x == 0) return 1.0;
		float PIx = PI * x;
		return A_DIV_PI2 * ((sin(PIx) * sin(PIx * A_INV)) / (x * x));
	}

	void main()
	{
		vec2 texelSize = vec2(1.0 / uSrcDimensions);
		vec2 pixCoord = uvCoord * uSrcDimensions;
		vec2 texCenter = floor(pixCoord) + vec2(0.5); // Coordinate to nearest texel center	
		vec2 texCenterOffs = pixCoord - texCenter;
		
		vec3 sum = vec3(0.0);
		float totalWeight = 0.0;
		for (int x = -N + 1; x <= N; ++x) {
			float xf = float(x);
			float xWeight = L(xf - texCenterOffs.x);
			
			for (int y = -N + 1; y <= N; ++y) {
				float yf = float(y);
				float yWeight = L(yf - texCenterOffs.y);
				vec2 coord = (texCenter + vec2(xf, yf)) * texelSize;
				sum += (xWeight * yWeight * texture(uSrcTex, coord).rgb);

				totalWeight += (xWeight * yWeight);
			}
		}
		
		outFragColor = vec4(sum / totalWeight, 1.0);
	}
)";

// Scaling algorithm enum
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

const char* to_string(ScalingAlgorithm algorithm) noexcept
{
	switch (algorithm) {
	case ScalingAlgorithm::NEAREST: return "NEAREST";
	case ScalingAlgorithm::BILINEAR: return "BILINEAR";
	case ScalingAlgorithm::GRID_2X2_NEAREST: return "GRID_2X2_NEAREST";
	case ScalingAlgorithm::GRID_2X2_BILINEAR: return "GRID_2X2_BILINEAR";
	case ScalingAlgorithm::GRID_4X4_NEAREST: return "GRID_4X4_NEAREST";
	case ScalingAlgorithm::GRID_4X4_BILINEAR: return "GRID_4X4_BILINEAR";
	case ScalingAlgorithm::BICUBIC_BSPLINE: return "BICUBIC_BSPLINE";
	case ScalingAlgorithm::LANCZOS_2: return "LANCZOS_2";
	case ScalingAlgorithm::LANCZOS_3: return "LANCZOS_3";
	case ScalingAlgorithm::LANCZOS_VARIABLE: return "LANCZOS_VARIABLE";
	default:
		sfz_assert_release_m(false, "Invalid or unhandled enum type.");
	}
}

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
		mProgram = gl::Program::postProcessFromSource(SINGLE_SAMPLE_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	case ScalingAlgorithm::BILINEAR:
		mProgram = gl::Program::postProcessFromSource(SINGLE_SAMPLE_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	case ScalingAlgorithm::GRID_2X2_NEAREST:
		mProgram = gl::Program::postProcessFromSource(GRID_2X2_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	case ScalingAlgorithm::GRID_2X2_BILINEAR:
		mProgram = gl::Program::postProcessFromSource(GRID_2X2_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	case ScalingAlgorithm::GRID_4X4_NEAREST:
		mProgram = gl::Program::postProcessFromSource(GRID_4X4_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	case ScalingAlgorithm::GRID_4X4_BILINEAR:
		mProgram = gl::Program::postProcessFromSource(GRID_4X4_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	case ScalingAlgorithm::BICUBIC_BSPLINE:
		mProgram = gl::Program::postProcessFromSource(BICUBIC_BSPLINE_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	case ScalingAlgorithm::LANCZOS_2:
		mProgram = gl::Program::postProcessFromSource(LANCZOS_VARIABLE_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	case ScalingAlgorithm::LANCZOS_3:
		mProgram = gl::Program::postProcessFromSource(LANCZOS_2_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	case ScalingAlgorithm::LANCZOS_VARIABLE:
		mProgram = gl::Program::postProcessFromSource(LANCZOS_3_SHADER_SRC);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glSamplerParameteri(mSamplerObject, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	default:
		sfz_assert_release_m(false, "Invalid scaling algorithm.");
	}
}

} // namespace sfz