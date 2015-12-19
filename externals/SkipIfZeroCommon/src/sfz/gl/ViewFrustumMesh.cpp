#include "sfz/gl/ViewFrustumMesh.hpp"

#include <algorithm>
#include <cmath>

#include "sfz/math/MathConstants.hpp"
#include "sfz/gl/OpenGL.hpp"


namespace gl {

using sfz::vec2;
using sfz::vec4;
using std::int32_t;

// ViewFrustumMesh: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ViewFrustumMesh::ViewFrustumMesh(const ViewFrustum& viewFrustum) noexcept
:
	ViewFrustumMesh(viewFrustum.verticalFov(), viewFrustum.aspectRatio(), viewFrustum.near(), viewFrustum.far())
{}

ViewFrustumMesh::ViewFrustumMesh(float verticalFovDeg, float aspect, float near, float far) noexcept
{
	/* // Original version
	const vec3 positions[] ={
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
	};*/

	const float vertFov = verticalFovDeg * sfz::DEG_TO_RAD();
	const float horizFov = vertFov * aspect;

	const float xNearOffs = near * std::tan(horizFov / 2.0f);
	const float xFarOffs = far * std::tan(horizFov / 2.0f);
	const float yNearOffs = near * std::tan(vertFov / 2.0f);
	const float yFarOffs = far * std::tan(vertFov / 2.0f);

	const float xDiff = xFarOffs - xNearOffs;
	const float yDiff = yFarOffs - yNearOffs;
	const float zDiff = far - near;
	const vec3 leftNormal = normalize(vec3{-xDiff, 0.0f, -zDiff});
	const vec3 rightNormal = normalize(vec3{xDiff, 0.0f, -zDiff});
	const vec3 bottomNormal = normalize(vec3{0.0f, -yDiff, -zDiff});
	const vec3 topNormal = normalize(vec3{0.0f, yDiff, -zDiff});
	const vec3 nearNormal = normalize(vec3{0.0f, 0.0f, -zDiff});
	const vec3 farNormal = normalize(vec3{0.0f, 0.0f, zDiff});

	const vec3 positions[] = {
		// Left (-x)
		vec3{-xFarOffs, yFarOffs, far},     // 0: 6, left-top-far
		vec3{-xNearOffs, -yNearOffs, near}, // 1: 0, left-bottom-near
		vec3{-xFarOffs, -yFarOffs, far},    // 2: 4, left-bottom-far
		vec3{-xNearOffs, yNearOffs, near},  // 3: 2, left-top-near

		// Right (+x)
		vec3{xNearOffs, yNearOffs, near},   // 4: 3, right-top-near
		vec3{xFarOffs, yFarOffs, far},      // 5: 7, right-top-far
		vec3{xNearOffs, -yNearOffs, near},  // 6: 1, right-bottom-near
		vec3{xFarOffs, -yFarOffs, far},     // 7: 5, right-bottom-far

		// Bottom (-y)
		vec3{-xNearOffs, -yNearOffs, near}, // 8: 0, left-bottom-near
		vec3{xNearOffs, -yNearOffs, near},  // 9: 1, right-bottom-near
		vec3{-xFarOffs, -yFarOffs, far},    // 10: 4, left-bottom-far
		vec3{xFarOffs, -yFarOffs, far},     // 11: 5, right-bottom-far

		// Top (+y)
		vec3{xNearOffs, yNearOffs, near},   // 12: 3, right-top-near
		vec3{-xNearOffs, yNearOffs, near},  // 13: 2, left-top-near
		vec3{-xFarOffs, yFarOffs, far},     // 14: 6, left-top-far
		vec3{xFarOffs, yFarOffs, far},      // 15: 7, right-top-far

		// Near (small z)
		vec3{-xNearOffs, -yNearOffs, near}, // 16: 0, left-bottom-near
		vec3{-xNearOffs, yNearOffs, near},  // 17: 2, left-top-near
		vec3{xNearOffs, yNearOffs, near},   // 18: 3, right-top-near
		vec3{xNearOffs, -yNearOffs, near},  // 19: 1, right-bottom-near

		// Far (large z)
		vec3{-xFarOffs, -yFarOffs, far},    // 20: 4, left-bottom-far
		vec3{xFarOffs, yFarOffs, far},      // 21: 7, right-top-far
		vec3{-xFarOffs, yFarOffs, far},     // 22: 6, left-top-far
		vec3{xFarOffs, -yFarOffs, far}      // 23: 5, right-bottom-far
	};
	const vec3 normals[] = {
		// Left (-x)
		leftNormal, leftNormal, leftNormal, leftNormal,
		
		// Right (+x)
		rightNormal, rightNormal, rightNormal, rightNormal,
		
		// Bottom (-y)
		bottomNormal, bottomNormal, bottomNormal, bottomNormal,
		
		// Top (+y)
		topNormal, topNormal, topNormal, topNormal,
		
		// Near (small z)
		nearNormal, nearNormal, nearNormal, nearNormal,

		// Far (large z)
		farNormal, farNormal, farNormal, farNormal
	};
	const vec2 uvCoords[] ={
		// Left (-x)
		vec2{0.0f, 0.0f}, // 0: 6, left-top-far
		vec2{1.0f, 0.0f}, // 1: 0, left-bottom-near
		vec2{0.0f, 1.0f}, // 2: 4, left-bottom-far
		vec2{1.0f, 1.0f}, // 3: 2, left-top-near

		// Right (+x)
		vec2{0.0f, 0.0f}, // 4: 3, right-top-near
		vec2{1.0f, 0.0f}, // 5: 7, right-top-far
		vec2{0.0f, 1.0f}, // 6: 1, right-bottom-near
		vec2{1.0f, 1.0f}, // 7: 5, right-bottom-far

		// Bottom (-y)
		vec2{0.0f, 0.0f}, // 8: 0, left-bottom-near
		vec2{1.0f, 0.0f}, // 9: 1, right-bottom-near
		vec2{0.0f, 1.0f}, // 10: 4, left-bottom-far
		vec2{1.0f, 1.0f}, // 11: 5, right-bottom-far

		// Top (+y)
		vec2{0.0f, 0.0f}, // 12: 3, right-top-near
		vec2{1.0f, 0.0f}, // 13: 2, left-top-near
		vec2{0.0f, 1.0f}, // 14: 6, left-top-far
		vec2{1.0f, 1.0f}, // 15: 7, right-top-far

		// Near (small z)
		vec2{0.0f, 0.0f}, // 16: 0, left-bottom-near
		vec2{1.0f, 0.0f}, // 17: 2, left-top-near
		vec2{0.0f, 1.0f}, // 18: 3, right-top-near
		vec2{1.0f, 1.0f}, // 19: 1, right-bottom-near

		// Far (large z)
		vec2{0.0f, 0.0f}, // 20: 4, left-bottom-far
		vec2{1.0f, 0.0f}, // 21: 7, right-top-far
		vec2{0.0f, 1.0f}, // 22: 6, left-top-far
		vec2{1.0f, 1.0f}  // 23: 5, right-bottom-far
	};
	const int32_t materialIDs[] ={
		// Left (-x)
		-1, -1, -1, -1,

		// Right (+x)
		-1, -1, -1, -1,

		// Bottom (-y)
		-1, -1, -1, -1,

		// Top (+y)
		-1, -1, -1, -1,

		// Near (small z)
		-1, -1, -1, -1,

		// Far (large z)
		-1, -1, -1, -1
	};
	const unsigned int indices[] ={
		// Left (-x)
		0, 1, 2,
		0, 3, 1,

		// Right (+x)
		4, 5, 6,
		6, 5, 7,

		// Bottom (-y)
		8, 9, 10,
		10, 9, 11,

		// Top (+y)
		12, 13, 14,
		12, 14, 15,

		// Near (small z)
		16, 17, 18,
		16, 18, 19,

		// Far (large z)
		20, 21, 22,
		20, 23, 21
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

mat4 ViewFrustumMesh::generateTransform(const ViewFrustum& viewFrustum) const noexcept
{
	return generateTransform(viewFrustum.pos(), viewFrustum.dir(), viewFrustum.up());
}

mat4 ViewFrustumMesh::generateTransform(vec3 pos, vec3 dir, vec3 up) const noexcept
{
	sfz_assert_debug(sfz::approxEqual(length(dir), 1.0f));
	sfz_assert_debug(sfz::approxEqual(length(up), 1.0f));
	mat4 tmp;
	tmp.setColumn(0, vec4{normalize(cross(up, dir)), 0.0f});
	tmp.setColumn(1, vec4{up, 0.0f});
	tmp.setColumn(2, vec4{dir, 0.0f});
	tmp.setColumn(3, vec4{pos, 1.0f});
	return tmp;
}

} // namespace gl