#include "sfz/gl/SpriteBatch.hpp"

#include <sfz/Assert.hpp>
#include <sfz/gl/Utils.hpp>

#include <new> // std::nothrow
#include <algorithm> // std::swap
#include <cmath>

namespace gl {

// Anonymous namespace
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

namespace {

// Anonymous: Shaders
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

const char* VERTEX_SHADER_SRC = R"(
	#version 330

	// Input
	in vec2 vertexIn;
	in mat3 transformIn;
	in vec4 uvCoordIn;

	// Ouput
	out vec2 uvCoord;

	void main()
	{
		gl_Position = vec4((transformIn*vec3(vertexIn, 1.0)).xy, 0.0, 1.0);
		switch (gl_VertexID) {
		case 0: uvCoord = uvCoordIn.xy; break;
		case 1: uvCoord = uvCoordIn.zy; break;
		case 2: uvCoord = uvCoordIn.xw; break;
		case 3: uvCoord = uvCoordIn.zw; break;
		}
	}
)";

const char* FRAGMENT_SHADER_SRC = R"(
	#version 330

	precision highp float; // required by GLSL spec Sect 4.5.3

	// Input
	in vec2 uvCoord;

	// Output
	out vec4 fragmentColor;

	// Uniforms
	uniform sampler2D uTexture;

	void main()
	{
		fragmentColor = texture(uTexture, uvCoord);
	}
)";

GLuint compileSpriteBatchShaderProgram(const char* vertexSrc, const char* fragmentSrc) noexcept
{
	GLuint vertexShader = gl::compileVertexShader(vertexSrc);
	GLuint fragmentShader = gl::compileFragmentShader(fragmentSrc);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glBindAttribLocation(shaderProgram, 0, "vertexIn");
	glBindAttribLocation(shaderProgram, 1, "transformIn");
	glBindAttribLocation(shaderProgram, 4, "uvCoordIn");
	glBindFragDataLocation(shaderProgram, 0, "fragmentColor");

	gl::linkProgram(shaderProgram);

	if (gl::checkAllGLErrors()) {
		std::cerr << "^^^ Above errors caused by shader compiling & linking." << std::endl;
	}

	return shaderProgram;
}

} // anonymous namespace

// SpriteBatch: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

SpriteBatch::SpriteBatch(size_t capacity) noexcept
: SpriteBatch(capacity, FRAGMENT_SHADER_SRC) { }


SpriteBatch::SpriteBatch(size_t capacity, const char* fragmentShaderSrc) noexcept
:
	mCapacity{capacity},
	mCurrentDrawCount{0},
	mShader{compileSpriteBatchShaderProgram(VERTEX_SHADER_SRC, fragmentShaderSrc)},
	mTransformArray{new (std::nothrow) mat3[mCapacity]},
	mUVArray{new (std::nothrow) vec4[mCapacity]}
{
	static_assert(sizeof(vec2) == sizeof(float)*2, "vec2 is padded");
	static_assert(sizeof(mat3) == sizeof(float)*9, "mat3 is padded");

	// Vertex buffer
	const float vertices[] = {
		-0.5f, -0.5f, // left bottom
		0.5f, -0.5f,  // right bottom
		-0.5f, 0.5f,  // left top
		0.5f, 0.5f    // right top
	};
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Transform buffer (null now, to be updated when rendering batch)
	glGenBuffers(1, &mTransformBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mTransformBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat3)*mCapacity, NULL, GL_STREAM_DRAW);

	// UV buffer (null now, to be updated when rendering batch)
	glGenBuffers(1, &mUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*mCapacity, NULL, GL_STREAM_DRAW);

	// Index buffer
	const unsigned int indices[] = {
		0, 1, 2,
		1, 3, 2
	};
	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Vertex Array Object
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, mTransformBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(mat3), (void*)(sizeof(float)*3*0));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(mat3), (void*)(sizeof(float)*3*1));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(mat3), (void*)(sizeof(float)*3*2));
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(4);

	if (gl::checkAllGLErrors()) {
		std::cerr << "^^^ Above errors caused by SpriteBatch constructor" << std::endl;
	}
}

SpriteBatch::SpriteBatch(SpriteBatch&& other) noexcept
:
	SpriteBatch(0)
{
	*this = std::move(other);
}

SpriteBatch& SpriteBatch::operator= (SpriteBatch&& other) noexcept
{
	std::swap(mCapacity, other.mCapacity);
	std::swap(mCurrentDrawCount, other.mCurrentDrawCount);
	std::swap(mCamProj, other.mCamProj);

	std::swap(mShader, other.mShader);
	std::swap(mVAO, other.mVAO);
	std::swap(mVertexBuffer, other.mVertexBuffer);
	std::swap(mIndexBuffer, other.mIndexBuffer);
	std::swap(mTransformBuffer, other.mTransformBuffer);
	std::swap(mUVBuffer, other.mUVBuffer);

	std::swap(mTransformArray, other.mTransformArray);
	std::swap(mUVArray, other.mUVArray);

	return *this;
}

SpriteBatch::~SpriteBatch() noexcept
{
	glDeleteProgram(mShader);
	glDeleteBuffers(1, &mVertexBuffer);
	glDeleteBuffers(1, &mIndexBuffer);
	glDeleteBuffers(1, &mTransformBuffer);
	glDeleteBuffers(1, &mUVBuffer);
	glDeleteVertexArrays(1, &mVAO);
}

// SpriteBatch: Public interface
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void SpriteBatch::begin(vec2 cameraPosition, vec2 cameraDimensions) noexcept
{
	mCamProj = sfz::glOrthogonalProjectionMatrix2D(cameraPosition, cameraDimensions);
	mCurrentDrawCount = 0;
}

void SpriteBatch::draw(vec2 position, vec2 dimensions, const TextureRegion& texRegion) noexcept
{
	mat3 transform{{dimensions[0], 0.0f, position[0]},
					{0.0f, dimensions[1], position[1]},
	                {0.0f, 0.0f, 1.0f}};

	// Setting transform & uv arrays
	mTransformArray[mCurrentDrawCount] = mCamProj * transform;
	mUVArray[mCurrentDrawCount] = vec4{texRegion.mUVMin[0], texRegion.mUVMin[1],
	                                    texRegion.mUVMax[0], texRegion.mUVMax[1]};

	// Incrementing current draw count
	mCurrentDrawCount++;
	sfz_assert_debug(mCurrentDrawCount <= mCapacity);
}

void SpriteBatch::draw(vec2 position, vec2 dimensions, float angleRads,
					   const TextureRegion& texRegion) noexcept
{
	float cos = std::cos(angleRads);
	float sin = std::sin(angleRads);
	mat3 rotMat{{cos, -sin, 0.0f}, {sin, cos, 0.0f}, {0.0f, 0.0f, 1.0f}};
	mat3 scaling{{dimensions[0], 0.0f, 0.0f}, {0.0f, dimensions[1], 0.0f}, {0.0f, 0.0f, 1.0f}};
	mat3 transform = rotMat * scaling;
	transform.setColumn(2, vec3{position[0], position[1], 1.0f});

	// Setting transform & uv arrays
	mTransformArray[mCurrentDrawCount] = mCamProj * transform;
	mUVArray[mCurrentDrawCount] = vec4{texRegion.mUVMin[0], texRegion.mUVMin[1],
	                                    texRegion.mUVMax[0], texRegion.mUVMax[1]};

	// Incrementing current draw count
	mCurrentDrawCount++;
	sfz_assert_debug(mCurrentDrawCount <= mCapacity);
}

void SpriteBatch::end(GLuint fbo, vec2 viewportDimensions, GLuint texture) noexcept
{
	sfz_assert_debug(mCurrentDrawCount <= mCapacity);

	// Setting up buffers and transferring data.
	// Uses orpahning, see: https://www.opengl.org/wiki/Buffer_Object_Streaming
	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribDivisor(0, 0); // Same quad for each draw instance

	glBindBuffer(GL_ARRAY_BUFFER, mTransformBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat3)*mCapacity, NULL, GL_STREAM_DRAW); // Orphaning.
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mat3)*mCurrentDrawCount, mTransformArray[0].data());
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(mat3), (void*)(sizeof(float)*3*0));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(mat3), (void*)(sizeof(float)*3*1));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(mat3), (void*)(sizeof(float)*3*2));
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(1, 1); // One transform per quad
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);

	glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*mCapacity, NULL, GL_STREAM_DRAW); // Orphaning.
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4)*mCurrentDrawCount, mUVArray[0].elements);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(4);
	glVertexAttribDivisor(4, 1); // One UV coordinate per vertex

	// Save previous depth test state and then disable it
	GLboolean depthTestWasEnabled;
	glGetBooleanv(GL_DEPTH_TEST, &depthTestWasEnabled);
	glDisable(GL_DEPTH_TEST);

	// Enabling shader
	glUseProgram(mShader);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, viewportDimensions[0], viewportDimensions[1]);

	// Uniforms
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	gl::setUniform(mShader, "uTexture", 0);

	// Drawing instances
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, mCurrentDrawCount);

	// Cleanup
	if (depthTestWasEnabled) glEnable(GL_DEPTH_TEST);
	glUseProgram(0); // TODO: Store previous program
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // TODO: Store previous framebuffer
	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 0);
	glVertexAttribDivisor(2, 0);
	glVertexAttribDivisor(3, 0);
	glVertexAttribDivisor(4, 0);
}


} // namespace sfz

