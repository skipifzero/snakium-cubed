#pragma once
#ifndef S3_RENDERING_GUI_RENDERING_HPP
#define S3_RENDERING_GUI_RENDERING_HPP

#include <sfz/gui/Button.hpp>
#include <sfz/math/Vector.hpp>

namespace s3 {

using gui::Button;
using sfz::vec2;
using sfz::vec3;
using sfz::vec4;

void renderButton(const Button& b, vec2 drawableDim, vec2 guiDim, vec2 guiOffs) noexcept;

} // namespace s3
#endif