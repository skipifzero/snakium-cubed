#pragma once
#ifndef SFZ_GEOMETRY_CIRCLE_HPP
#define SFZ_GEOMETRY_CIRCLE_HPP

#include <functional> // std::hash
#include <string>
#include <cmath> // std::abs
#include <cassert>
#include "sfz/math/MathConstants.hpp"
#include "sfz/math/Vector.hpp"

// Forward declares Rectangle, is included after complete declaration of Circle.
namespace sfz { class Rectangle; }

namespace sfz {

/**
 * @brief A POD class representing a Circle.
 * @author Peter Hillerström <peter@hstroem.se>
 */
struct Circle final {
	
	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	vec2 pos;
	float radius;

	// Constructors and destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline Circle() noexcept = default;
	inline Circle(const Circle&) noexcept = default;
	inline Circle& operator= (const Circle&) noexcept = default;
	inline ~Circle() noexcept = default;

	inline Circle(vec2 pos, float radius) noexcept;
	inline Circle(float x, float y, float radius) noexcept;

	// Public member functions
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline bool overlap(vec2 point) const noexcept;
	inline bool overlap(const Circle& circle) const noexcept;
	inline bool overlap(const Rectangle& rect) const noexcept;
	inline float area() const noexcept;
	inline float circumference() const noexcept;

	inline size_t hash() const noexcept;
	inline std::string to_string() const noexcept;

	// Comparison operators
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline bool operator== (const Circle& other) const noexcept;
	inline bool operator!= (const Circle& other) const noexcept;
};

// Free (non-member) operators
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
inline std::ostream& operator<< (std::ostream& ostream, const Circle circle) noexcept;

} // namespace sfz

// Specializations of standard library for sfz::Circle
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
namespace std {

struct hash<sfz::Circle> {
	inline size_t operator() (const sfz::Circle& circle) const noexcept;
};

} // namespace std

#include "sfz/Rectangle.hpp"
#include "Circle.inl"
#endif