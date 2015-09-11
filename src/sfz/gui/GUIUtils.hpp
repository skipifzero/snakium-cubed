#pragma once
#ifndef SFZ_GUI_GUI_UTILS_HPP
#define SFZ_GUI_GUI_UTILS_HPP

#include "sfz/geometry/AABB2D.hpp"
#include "sfz/math/Vector.hpp"

namespace gui {

using sfz::AABB2D;
using sfz::vec2;

/** 
 * @brief Calculates camera for a GUI system with the given parameters.
 * The algorithm will fit the minDrawable box as well as possible within the given drawable area.
 * When drawing the gui one can then use the coordinate system [(0,0), minDrawable] when drawing
 * the gui and it will display correctly on all window sizes.
 */
AABB2D calculateGUICamera(vec2 drawableDim, vec2 minDrawable) noexcept;

} // namespace gui
#endif