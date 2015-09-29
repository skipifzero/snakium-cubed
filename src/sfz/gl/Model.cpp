#include "sfz/gl/Model.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "tiny_obj_loader.h"

#include "sfz/gl/OpenGL.hpp"

namespace gl {

using std::string;
using std::vector;

using tinyobj::shape_t;
using tinyobj::material_t;

// Model: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Model::Model(const char* basePath, const char* filename) noexcept
{
	vector<shape_t> shapes;
	vector<material_t> materials;

	string error = tinyobj::LoadObj(shapes, materials, (string(basePath) + filename).c_str(), basePath);

	if (!error.empty()) {
		std::cerr << error << std::endl;
		return;
	}

	// TODO: Temp hack
	if (shapes.size() > 1) {
		std::cerr << filename << ": has more than 1 shape.\n";
		std::terminate();
	}
	if (shapes[0].mesh.texcoords.size() > 0) {
		std::cerr << filename << ": has uv coords\n";
		std::terminate();
	}


	/*for (auto& shape : shapes) {

		std::cout << "Positions:\n";
		for (size_t i = 0; i < shape.mesh.positions.size(); i += 3) {
			std::cout << "[" << (i/3) << "]: {" << shape.mesh.positions[i] << ", "
			          << shape.mesh.positions[i+1] << ", " << shape.mesh.positions[i+2] << "}\n";
		}
		
		std::cout << "\nNormals:\n";
		for (size_t i = 0; i < shape.mesh.normals.size(); i += 3) {
			std::cout << "[" << (i/3) << "]: {" << shape.mesh.normals[i] << ", "
			          << shape.mesh.normals[i+1] << ", " << shape.mesh.normals[i+2] << "}\n";
		}

		std::cout << "\nTexCoords:\n";
		for (size_t i = 0; i < shape.mesh.texcoords.size(); i += 2) {
			std::cout << "[" << (i/2) << "]: {" << shape.mesh.texcoords[i] << ", "
			          << shape.mesh.texcoords[i+1] << "}\n";
		}

		std::cout << "\nIndices:\n";
		for (size_t i = 0; i < shape.mesh.indices.size(); ++i) {
			std::cout << "[" << i << "]: " << shape.mesh.indices[i] << "\n";
		}
	}*/

	// Creates Vertex Array Object
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	auto& shape = shapes[0];

	// Buffer objects
	glGenBuffers(1, &mPositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mPositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, shape.mesh.positions.size()*sizeof(float), shape.mesh.positions.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &mNormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, shape.mesh.normals.size()*sizeof(float), shape.mesh.normals.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &mUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, shape.mesh.texcoords.size()*sizeof(float), shape.mesh.texcoords.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ARRAY_BUFFER, shape.mesh.indices.size()*sizeof(unsigned int), shape.mesh.indices.data(), GL_STATIC_DRAW);
	mNumIndices = shape.mesh.indices.size();

	// Bind buffers to VAO
	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mPositionBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
}

Model::Model(Model&& other) noexcept
{
	std::swap(this->mVAO, other.mVAO);
	std::swap(this->mPositionBuffer, other.mPositionBuffer);
	std::swap(this->mNormalBuffer, other.mNormalBuffer);
	std::swap(this->mUVBuffer, other.mUVBuffer);
	std::swap(this->mIndexBuffer, other.mIndexBuffer);
	std::swap(this->mNumIndices, other.mNumIndices);
}

Model& Model::operator= (Model&& other) noexcept
{
	std::swap(this->mVAO, other.mVAO);
	std::swap(this->mPositionBuffer, other.mPositionBuffer);
	std::swap(this->mNormalBuffer, other.mNormalBuffer);
	std::swap(this->mUVBuffer, other.mUVBuffer);
	std::swap(this->mIndexBuffer, other.mIndexBuffer);
	std::swap(this->mNumIndices, other.mNumIndices);
	return *this;
}

Model::~Model() noexcept
{
	// Silently ignores values == 0
	glDeleteBuffers(1, &mPositionBuffer);
	glDeleteBuffers(1, &mNormalBuffer);
	glDeleteBuffers(1, &mUVBuffer);
	glDeleteBuffers(1, &mIndexBuffer);
	glDeleteVertexArrays(1, &mVAO);

}

// Model: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Model::render() noexcept
{
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glDrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 0);
}

} // namespace gl