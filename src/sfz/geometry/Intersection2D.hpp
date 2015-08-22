#pragma once
#ifndef SFZ_GEOMETRY_INTERSECTION_2D_HPP
#define SFZ_GEOMETRY_INTERSECTION_2D_HPP

#include "sfz/math/Vector.hpp"
#include "sfz/geometry/Circle.hpp"
#include "sfz/geometry/Rectangle.hpp"

namespace sfz {

bool pointInside(const Circle& circle, vec2 point) noexcept;
bool pointInside(const Rectangle& rect, vec2 point) noexcept;

bool overlaps(const Circle& lhs, const Circle& rhs) noexcept;
bool overlaps(const Rectangle& lhs, const Rectangle& rhs) noexcept;
bool overlaps(const Circle& circle, const Rectangle& rect) noexcept;
bool overlaps(const Rectangle& rect, const Circle& circle) noexcept;

} // namespace sfz
#endif