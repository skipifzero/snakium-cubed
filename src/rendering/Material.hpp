#pragma once
#ifndef S3_RENDERING_MATERIAL_HPP
#define S3_RENDERING_MATERIAL_HPP

#include <sfz/math/Vector.hpp>
#include <sfz/gl/Program.hpp>

namespace s3 {

using sfz::vec3;

struct Material final {
	vec3 ambient = vec3{0.025f};
	vec3 diffuse = vec3{0.0f};
	vec3 specular = vec3{0.25f};
	vec3 emissive = vec3{0.0f};
	float shininess = 6.0f;
	float opaque = 1.0f;
};

/** Warning, a setUniform() call per member. Probably slow. */
void setUniformMaterial(const gl::Program& program, const char* name, const Material& material) noexcept;

} // namespace s3
#endif