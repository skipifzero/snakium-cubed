#pragma once
#ifndef SFZ_GEOMETRY_CIRCLE_HPP
#define SFZ_GEOMETRY_CIRCLE_HPP

#include <functional> // std::hash
#include <string>
#include <cmath> // std::abs
#include "sfz/math/MathConstants.hpp"
#include "sfz/math/Vector.hpp"

namespace sfz {

/** @brief A POD class representing a Circle. */
struct Circle final {
	
	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	vec2 pos;
	float radius;

	// Constructors and destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Circle() noexcept = default;
	Circle(const Circle&) noexcept = default;
	Circle& operator= (const Circle&) noexcept = default;
	~Circle() noexcept = default;

	inline Circle(vec2 centerPos, float radius) noexcept;
	inline Circle(float centerX, float centerY, float radius) noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline size_t hash() const noexcept;
	inline std::string to_string() const noexcept;

	// Public getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline float x() const noexcept { return pos.x; }
	inline float y() const noexcept { return pos.y; }

	// Comparison operators
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline bool operator== (const Circle& other) const noexcept;
	inline bool operator!= (const Circle& other) const noexcept;
};

// Free (non-member) operators
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
inline std::ostream& operator<< (std::ostream& ostream, const Circle& circle) noexcept;

} // namespace sfz

// Specializations of standard library for sfz::Circle
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
namespace std {

template<>
struct hash<sfz::Circle> {
	inline size_t operator() (const sfz::Circle& circle) const noexcept;
};

} // namespace std
#include "sfz/geometry/Circle.inl"
#endif