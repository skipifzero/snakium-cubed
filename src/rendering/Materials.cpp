#include "rendering/Materials.hpp"

#include <cstdio>

namespace s3 {

const Material* getMaterials() noexcept
{
	static Material materials[NUM_MATERIAL_IDS];

	materials[MATERIAL_ID_TILE_OBJECT].diffuse = vec3{0.0f, 1.0f, 1.0f} * 0.25f;
	materials[MATERIAL_ID_TILE_OBJECT].emissive = vec3{0.0f, 1.0f, 1.0f} * 0.75f;

	materials[MATERIAL_ID_TILE_BONUS_OBJECT].diffuse = vec3{1.0f, 0.0f, 0.85f} * 0.25f;
	materials[MATERIAL_ID_TILE_BONUS_OBJECT].emissive = vec3{1.0f, 0.0f, 0.85f} * 0.75f;
	
	materials[MATERIAL_ID_TILE_SNAKE].diffuse = vec3{0.0f, 1.0f, 0.25f} * 0.25f;
	materials[MATERIAL_ID_TILE_SNAKE].emissive = vec3{0.0f, 1.0f, 0.25f} * 0.75f;

	materials[MATERIAL_ID_TILE_SNAKE_DIG].diffuse = vec3{0.0f, 1.0f, 0.25f} * 0.25f;
	materials[MATERIAL_ID_TILE_SNAKE_DIG].emissive = vec3{0.0f, 1.0f, 0.25f} * 0.75f;

	materials[MATERIAL_ID_TILE_DECORATION].diffuse = vec3{0.25f, 0.5f, 0.5f};
	materials[MATERIAL_ID_TILE_DECORATION].emissive = vec3{0.25f, 0.5f, 0.5f} * 0.4f;

	materials[MATERIAL_ID_TILE_DECORATION_OCCUPIED].diffuse = vec3{0.0f, 1.0f, 0.25f};
	materials[MATERIAL_ID_TILE_DECORATION_OCCUPIED].emissive = vec3{0.0f, 1.0f, 0.25f} * 0.4f;

	materials[MATERIAL_ID_TILE_DIVE_ASCEND].diffuse = vec3{0.5f, 0.0f, 0.75f} * 0.25f;
	materials[MATERIAL_ID_TILE_DIVE_ASCEND].emissive = vec3{0.5f, 0.0f, 0.75f} * 0.6f;

	materials[MATERIAL_ID_TILE_PROJECTION].diffuse = vec3{0.5f, 0.5f, 0.5f} * 0.4f;
	materials[MATERIAL_ID_TILE_PROJECTION].emissive = vec3{0.1f};
	materials[MATERIAL_ID_TILE_PROJECTION].opaque = 0.8f;

	materials[MATERIAL_ID_CUBE_SIDE].diffuse = vec3{0.25f, 0.25f, 0.25f};
	materials[MATERIAL_ID_CUBE_SIDE].opaque = 0.6f;

	materials[MATERIAL_ID_SKY].diffuse = vec3{0.2f, 0.2f, 0.2f};

	materials[MATERIAL_ID_GROUND].diffuse = vec3{0.5f, 0.5f, 0.5f};

	return materials;
}

void stupidSetUniformMaterials(const gl::Program& program, const char* name,
                               const Material* materials, uint32_t numMaterials) noexcept
{
	char buffer[128];
	for (uint32_t i = 0; i < numMaterials; ++i) {
		std::snprintf(buffer, sizeof(buffer), "%s[%u].%s", name, i, "diffuse");
		gl::setUniform(program, buffer, materials[i].diffuse);
		std::snprintf(buffer, sizeof(buffer), "%s[%u].%s", name, i, "specular");
		gl::setUniform(program, buffer, materials[i].specular);
		std::snprintf(buffer, sizeof(buffer), "%s[%u].%s", name, i, "emissive");
		gl::setUniform(program, buffer, materials[i].emissive);
		std::snprintf(buffer, sizeof(buffer), "%s[%u].%s", name, i, "shininess");
		gl::setUniform(program, buffer, materials[i].shininess);
		std::snprintf(buffer, sizeof(buffer), "%s[%u].%s", name, i, "opaque");
		gl::setUniform(program, buffer, materials[i].opaque);
	}
}

} // namespace s3