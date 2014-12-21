#pragma once
#ifndef S3_TILE_OBJECT_HPP
#define S3_TILE_OBJECT_HPP

#include <sfz/Math.hpp>
#include "sfz/GL.hpp"

namespace s3 {

class TileObject final {
public:
	TileObject(bool xFlipped, bool yFlipped) noexcept;
	~TileObject() noexcept;
	void render() noexcept;

	TileObject() = delete;
	TileObject(const TileObject&) = delete;
	TileObject& operator= (const TileObject&) = delete;
private:
	GLuint vertexArrayObject;
	GLuint posBuffer, uvBuffer, indexBuffer;
};

} // namespace s3

#endif
