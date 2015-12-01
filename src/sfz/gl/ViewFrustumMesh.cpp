#include "sfz/gl/ViewFrustumMesh.hpp"

#include <algorithm>
#include <cmath>

#include "sfz/math/MathConstants.hpp"
#include "sfz/gl/GLUtils.hpp"
#include "sfz/gl/OpenGL.hpp"


namespace gl {

// ViewFrustumMesh: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ViewFrustumMesh::ViewFrustumMesh(float verticalFovDeg, float aspect, float near, float far) noexcept
{
	float vertFov = verticalFovDeg * sfz::DEG_TO_RAD();
	float horizFov = vertFov * aspect;

	const float xNearOffs = near * std::tan(horizFov / 2.0f);
	const float xFarOffs = far * std::tan(horizFov / 2.0f);
	const float yNearOffs = near * std::tan(vertFov / 2.0f);
	const float yFarOffs = far * std::tan(vertFov / 2.0f);

	const vec3 positions[] = {
		// Left (-x)
	//	vec3{-xFarOffs, yFarOffs, far},  // 6, left-top-far
	//	vec3{-xNearOffs, -yNearOffs, near}, // 0, left-bottom-near
	//	vec3{-xFarOffs, -yFarOffs, far}, // 4, left-bottom-far
	//	vec3{-xNearOffs, yNearOffs, near},  // 2, left-top-near

		// Right (+x)

		// Bottom (-y)

		// Top (+y)

		// Near (small z)

		// Far (large z)

		// Near
		vec3{-xNearOffs, -yNearOffs, near}, // 0, left-bottom-near
		vec3{xNearOffs, -yNearOffs, near},  // 1, right-bottom-near
		vec3{-xNearOffs, yNearOffs, near},  // 2, left-top-near
		vec3{xNearOffs, yNearOffs, near},   // 3, right-top-near
		// Far
		vec3{-xFarOffs, -yFarOffs, far}, // 4, left-bottom-far
		vec3{xFarOffs, -yFarOffs, far},  // 5, right-bottom-far
		vec3{-xFarOffs, yFarOffs, far},  // 6, left-top-far
		vec3{xFarOffs, yFarOffs, far}    // 7, right-top-far
	};
	/*const unsigned int indices[] ={
		// Left (-x)
		0, 1, 2,
		0, 3, 1

		// Right (+x)

		// Bottom (-y)

		// Top (+y)

		// Near (small z)

		// Far (large z)
	};*/

	
	

	const float normals[] ={
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};
	const float uvCoords[] ={
		// bottom-left UV
		0.0f, 0.0f,
		// bottom-right UV
		1.0f, 0.0f,
		// top-left UV
		0.0f, 1.0f,
		// top-right UV
		1.0f, 1.0f,

		// bottom-left UV
		0.0f, 0.0f,
		// bottom-right UV
		1.0f, 0.0f,
		// top-left UV
		0.0f, 1.0f,
		// top-right UV
		1.0f, 1.0f
	};
	const int32_t materialIDs[] ={
		-1, -1, -1, -1,
		-1, -1, -1, -1
	};
	const unsigned int indices[] ={
		// Back
		0, 2, 3,
		0, 3, 1,

		// Front
		4, 7, 6,
		4, 5, 7,

		// Left
		6, 0, 4,
		6, 2, 0,

		// Right
		3, 7, 1,
		1, 7, 5,

		// Top
		3, 2, 6,
		3, 6, 7,

		// Bottom
		0, 1, 4,
		4, 1, 5
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

	if (gl::checkAllGLErrors()) {
		std::cerr << "^^^ Above errors likely caused by ViewFrustumMesh constructor." << std::endl;
	}
}

ViewFrustumMesh::ViewFrustumMesh(ViewFrustumMesh&& other) noexcept
{
	std::swap(this->mVAO, other.mVAO);
	std::swap(this->mPosBuffer, other.mPosBuffer);
	std::swap(this->mNormalBuffer, other.mNormalBuffer);
	std::swap(this->mUVBuffer, other.mUVBuffer);
	std::swap(this->mMaterialIDBuffer, other.mMaterialIDBuffer);
	std::swap(this->mIndexBuffer, other.mIndexBuffer);
}

ViewFrustumMesh& ViewFrustumMesh::operator= (ViewFrustumMesh&& other) noexcept
{
	std::swap(this->mVAO, other.mVAO);
	std::swap(this->mPosBuffer, other.mPosBuffer);
	std::swap(this->mNormalBuffer, other.mNormalBuffer);
	std::swap(this->mUVBuffer, other.mUVBuffer);
	std::swap(this->mMaterialIDBuffer, other.mMaterialIDBuffer);
	std::swap(this->mIndexBuffer, other.mIndexBuffer);
	return *this;
}

ViewFrustumMesh::~ViewFrustumMesh() noexcept
{
	glDeleteBuffers(1, &mPosBuffer);
	glDeleteBuffers(1, &mNormalBuffer);
	glDeleteBuffers(1, &mUVBuffer);
	glDeleteBuffers(1, &mMaterialIDBuffer);
	glDeleteBuffers(1, &mIndexBuffer);
	glDeleteVertexArrays(1, &mVAO);
}

// ViewFrustumMesh: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void ViewFrustumMesh::render() noexcept
{
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
}

} // namespace gl