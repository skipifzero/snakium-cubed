#pragma once
#ifndef S3_RENDERING_MATERIALS_HPP
#define S3_RENDERING_MATERIALS_HPP

#include <cstdint>

#include <sfz/math/Vector.hpp>
#include <sfz/gl/Program.hpp>

namespace s3 {

using sfz::vec3;
using std::uint32_t;

// Material struct
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct Material final {
	vec3 diffuse = vec3{0.0f};
	vec3 specular = vec3{0.25f};
	vec3 emissive = vec3{0.0f};
	float shininess = 6.0f;
	float opaque = 1.0f;
};

// Material index constants
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

const uint32_t MATERIAL_ID_DEFAULT = 0;
const uint32_t MATERIAL_ID_TILE_OBJECT = 1;
const uint32_t MATERIAL_ID_TILE_BONUS_OBJECT = 2;
const uint32_t MATERIAL_ID_TILE_SNAKE = 3;
const uint32_t MATERIAL_ID_TILE_SNAKE_DIG = 4;
const uint32_t MATERIAL_ID_TILE_DECORATION = 5;
const uint32_t MATERIAL_ID_TILE_DECORATION_OCCUPIED = 6;
const uint32_t MATERIAL_ID_TILE_DIVE_ASCEND = 7;
const uint32_t MATERIAL_ID_TILE_PROJECTION = 8;
const uint32_t MATERIAL_ID_CUBE_SIDE = 9;
const uint32_t MATERIAL_ID_SKY = 10;
const uint32_t MATERIAL_ID_GROUND = 11;

const uint32_t NUM_MATERIAL_IDS = 12;

// Material definitions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

const Material* getMaterials() noexcept;

/** Warning, stupidly slow. Should really use a uniform buffer instead. */
void stupidSetUniformMaterials(const gl::Program& program, const char* name,
                               const Material* materials = getMaterials(),
                               uint32_t numMaterials = NUM_MATERIAL_IDS) noexcept;

} // namespace s3
#endif