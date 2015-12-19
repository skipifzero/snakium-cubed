#include "sfz/gl/PostProcessQuad.hpp"

#include <algorithm>
#include <iostream>

#include "sfz/gl/OpenGL.hpp"

namespace gl {

using std::int32_t;

PostProcessQuad::PostProcessQuad() noexcept
{
	const float positions[] = {
		-1.0f, -1.0f, 0.0f, // bottom-left
		1.0f, -1.0f, 0.0f, // bottom-right
		-1.0f, 1.0f, 0.0f, // top-left
		1.0f, 1.0f, 0.0f // top-right
	};
	const float normals[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};
	const float uvCoords[] = {
		// bottom-left UV
		0.0f, 0.0f,
		// bottom-right UV
		1.0f, 0.0f,
		// top-left UV
		0.0f, 1.0f,
		// top-right UV
		1.0f, 1.0f
	};
	const int32_t materialIDs[] = {
		-1, -1, -1, -1
	};
	const unsigned int indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	// Buffer objects
	glGenBuffers(1, &mPosBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mPosBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glGenBuffers(1, &mNormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

	glGenBuffers(1, &mUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvCoords), uvCoords, GL_STATIC_DRAW);

	glGenBuffers(1, &mMaterialIDBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mMaterialIDBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(materialIDs), materialIDs, GL_STATIC_DRAW);

	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Vertex Array Object
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mPosBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, mMaterialIDBuffer);
	glVertexAttribPointer(3, 1, GL_INT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);
}

PostProcessQuad::PostProcessQuad(PostProcessQuad&& other) noexcept
{
	std::swap(this->mVAO, other.mVAO);
	std::swap(this->mPosBuffer, other.mPosBuffer);
	std::swap(this->mNormalBuffer, other.mNormalBuffer);
	std::swap(this->mUVBuffer, other.mUVBuffer);
	std::swap(this->mMaterialIDBuffer, other.mMaterialIDBuffer);
	std::swap(this->mIndexBuffer, other.mIndexBuffer);
}

PostProcessQuad& PostProcessQuad::operator= (PostProcessQuad&& other) noexcept
{
	std::swap(this->mVAO, other.mVAO);
	std::swap(this->mPosBuffer, other.mPosBuffer);
	std::swap(this->mNormalBuffer, other.mNormalBuffer);
	std::swap(this->mUVBuffer, other.mUVBuffer);
	std::swap(this->mMaterialIDBuffer, other.mMaterialIDBuffer);
	std::swap(this->mIndexBuffer, other.mIndexBuffer);
	return *this;
}

PostProcessQuad::~PostProcessQuad() noexcept
{
	glDeleteBuffers(1, &mPosBuffer);
	glDeleteBuffers(1, &mNormalBuffer);
	glDeleteBuffers(1, &mUVBuffer);
	glDeleteBuffers(1, &mMaterialIDBuffer);
	glDeleteBuffers(1, &mIndexBuffer);
	glDeleteVertexArrays(1, &mVAO);
}

void PostProcessQuad::render() noexcept
{
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

} // namespace gl