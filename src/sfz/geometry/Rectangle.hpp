#pragma once
#ifndef SFZ_GEOMETRY_RECTANGLE_HPP
#define SFZ_GEOMETRY_RECTANGLE_HPP

#include <functional> // std::hash
#include <string>
#include <iostream> // ostream
#include <cmath> // std::abs
#include "sfz/math/Vector.hpp"

// Forward declares Circle, is included after complete declaration of Rectangle.
namespace sfz { struct Circle; }

namespace sfz {

/**
 * @brief A POD class representing a Rectangle.
 * @author Peter Hillerström <peter@hstroem.se>
 */
struct Rectangle final {
	
	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	vec2 pos, dim;

	// Constructors and destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline Rectangle() noexcept = default;
	inline Rectangle(const Rectangle&) noexcept = default;
	inline Rectangle& operator= (const Rectangle&) noexcept = default;
	inline ~Rectangle() noexcept = default;

	inline Rectangle(vec2 position, vec2 dimensions) noexcept;
	inline Rectangle(float x, float y, float width, float height) noexcept;

	// Public member functions
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline bool overlap(vec2 point) const noexcept;
	inline bool overlap(const Rectangle& rect) const noexcept;
	inline bool overlap(const Circle& circle) const noexcept;

	inline size_t hash() const noexcept;
	inline std::string to_string() const noexcept;

	// Comparison operators
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline bool operator== (const Rectangle& other) const noexcept;
	inline bool operator!= (const Rectangle& other) const noexcept;
};

// Free (non-member) operators
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline std::ostream& operator<< (std::ostream& ostream, const Rectangle rect) noexcept;

} // namespace sfz

// Specializations of standard library for sfz::Rectangle
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
namespace std {

template<>
struct hash<sfz::Rectangle> {
	inline size_t operator() (const sfz::Rectangle& rect) const noexcept;
};

} // namespace std

#include "sfz/geometry/Circle.hpp"
#include "sfz/geometry/Rectangle.inl"
#endif