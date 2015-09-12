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

/**
 * Calculates a new viewport to be used for rendering to a specific part of the window using
 * "the same" camera as before. It is assumed that camera and oldViewport directly map to
 * each other, so by defining an area in the cameras coordinate system this same area can be
 * transformed into viewport space, which is what this function does.
 * @param oldViewport the old viewport
 * @param camera the camera defining the current coordinate system
 * @param viewportArea a rectangle in the same coordinate system as the camera, defining where
 * and how big the viewport should be relative to the old viewport
 */
AABB2D calculateViewport(const AABB2D& oldViewport, const AABB2D& camera,
                         const AABB2D& viewportArea) noexcept;

} // namespace gui
#endif