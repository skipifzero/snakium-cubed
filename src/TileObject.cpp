#include "TileObject.hpp"

namespace  s3 {

TileObject::TileObject() noexcept
{
	const float positions[] = {
		-0.5f, 0.0f, 0.5f, // bottom-left
		0.5f, 0.0f, 0.5f, // bottom-right
		-0.5f, 0.0f, -0.5f, // top-left
		0.5f, 0.0f, -0.5f // top-right
	};
	const float uvCoords[] = {
		// u,    v
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};
	const unsigned int indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	// Buffer objects
	glGenBuffers(1, &posBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvCoords), uvCoords, GL_STATIC_DRAW);

	// Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);

	if (gl::checkAllGLErrors()) {
		std::cerr << "^^^ Above errors likely caused by TileObject ctor." << std::endl;
	}
}

TileObject::~TileObject() noexcept
{
	glDeleteBuffers(1, &posBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteVertexArrays(1, &vertexArrayObject);
}

void TileObject::render() noexcept
{
	glBindVertexArray(vertexArrayObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

} // namespace s3