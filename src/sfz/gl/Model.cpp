#include "sfz/gl/Model.hpp"

#include <algorithm>
#include <iostream>
#include <new>
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

Model::Model(const char* basePath, const char* filename, bool requireNormals,
             bool requireUVs) noexcept
{
	vector<shape_t> shapes;
	vector<material_t> materials;

	string error = tinyobj::LoadObj(shapes, materials, (string(basePath) + filename).c_str(), basePath);

	if (!error.empty()) {
		std::cerr << error << std::endl;
		return;
	}

	// Make sure shapes has required properties
	if (shapes.size() == 0) {
		std::cerr << "Model \"" << filename << "\" has no shapes\n";
		return;
	}
	if (requireNormals) {
		for (size_t i = 0; i < shapes.size(); ++i) {
			if (shapes[i].mesh.normals.size() == 0) {
				std::cerr << "Model \"" << filename << "\" shape " << i << " has no normals\n";
				return;
			}
		}
	}
	if (requireUVs) {
		for (size_t i = 0; i < shapes.size(); ++i) {
			if (shapes[i].mesh.texcoords.size() == 0) {
				std::cerr << "Model \"" << filename << "\" shape " << i << " has no UV coords\n";
				return;
			}
		}
	}

	mVAORenderingInfos = unique_ptr<VAORenderingInfo[]>{new (std::nothrow) VAORenderingInfo[shapes.size()]};
	mVAOBufferInfos = unique_ptr<VAOBufferInfo[]>{new (std::nothrow) VAOBufferInfo[shapes.size()]};

	for (size_t i = 0; i < shapes.size(); ++i) {
		auto& shape = shapes[i];
		auto& renderingInfo = mVAORenderingInfos[i];
		auto& bufferInfo = mVAOBufferInfos[i];

		// Creates Vertex Array Object
		glGenVertexArrays(1, &renderingInfo.vao);
		glBindVertexArray(renderingInfo.vao);

		// Buffer objects
		glGenBuffers(1, &bufferInfo.positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, shape.mesh.positions.size()*sizeof(float), shape.mesh.positions.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &bufferInfo.normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, shape.mesh.normals.size()*sizeof(float), shape.mesh.normals.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &bufferInfo.uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, shape.mesh.texcoords.size()*sizeof(float), shape.mesh.texcoords.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &renderingInfo.indexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, renderingInfo.indexBuffer);
		glBufferData(GL_ARRAY_BUFFER, shape.mesh.indices.size()*sizeof(unsigned int), shape.mesh.indices.data(), GL_STATIC_DRAW);
		renderingInfo.numIndices = shape.mesh.indices.size();

		glGenBuffers(1, &bufferInfo.materialIDBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.materialIDBuffer);
		glBufferData(GL_ARRAY_BUFFER, shape.mesh.material_ids.size()*sizeof(int), shape.mesh.material_ids.data(), GL_STATIC_DRAW);
		
		// Bind buffers to VAO
		glBindVertexArray(renderingInfo.vao);

		glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.positionBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.normalBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.uvBuffer);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.materialIDBuffer);
		glVertexAttribPointer(3, 1, GL_INT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(3);
	}

	mNumVAOs = shapes.size();
}

Model::Model(Model&& other) noexcept
{
	std::swap(this->mVAORenderingInfos, other.mVAORenderingInfos);
	std::swap(this->mVAOBufferInfos, other.mVAOBufferInfos);
}

Model& Model::operator= (Model&& other) noexcept
{
	std::swap(this->mVAORenderingInfos, other.mVAORenderingInfos);
	std::swap(this->mVAOBufferInfos, other.mVAOBufferInfos);
	return *this;
}

// Model: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Model::render() noexcept
{
	for (size_t i = 0; i < mNumVAOs; ++i) {
		const VAORenderingInfo& info = mVAORenderingInfos[i];
		glBindVertexArray(info.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, info.indexBuffer);
		glDrawElements(GL_TRIANGLES, info.numIndices, GL_UNSIGNED_INT, 0);
	}
}

// Model: Private classes
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Model::VAORenderingInfo::VAORenderingInfo(VAORenderingInfo&& other) noexcept
{
	std::swap(this->vao, other.vao);
	std::swap(this->indexBuffer, other.indexBuffer);
	std::swap(this->numIndices, other.numIndices);
}

Model::VAORenderingInfo& Model::VAORenderingInfo::operator= (VAORenderingInfo&& other) noexcept
{
	std::swap(this->vao, other.vao);
	std::swap(this->indexBuffer, other.indexBuffer);
	std::swap(this->numIndices, other.numIndices);
	return *this;
}

Model::VAORenderingInfo::~VAORenderingInfo() noexcept
{
	// Silently ignores values == 0.
	glDeleteBuffers(1, &indexBuffer);
	glDeleteVertexArrays(1, &vao);
}


Model::VAOBufferInfo::VAOBufferInfo(VAOBufferInfo&& other) noexcept
{
	std::swap(this->positionBuffer, other.positionBuffer);
	std::swap(this->normalBuffer, other.normalBuffer);
	std::swap(this->uvBuffer, other.uvBuffer);
	std::swap(this->materialIDBuffer, other.materialIDBuffer);
}

Model::VAOBufferInfo& Model::VAOBufferInfo::operator= (VAOBufferInfo&& other) noexcept
{
	std::swap(this->positionBuffer, other.positionBuffer);
	std::swap(this->normalBuffer, other.normalBuffer);
	std::swap(this->uvBuffer, other.uvBuffer);
	std::swap(this->materialIDBuffer, other.materialIDBuffer);
	return *this;
}

Model::VAOBufferInfo::~VAOBufferInfo() noexcept
{
	// Silently ignores values == 0
	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &materialIDBuffer);
}

} // namespace gl