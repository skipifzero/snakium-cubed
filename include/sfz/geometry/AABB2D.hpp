#pragma once
#ifndef SFZ_GEOMETRY_AABB_2D_HPP
#define SFZ_GEOMETRY_AABB_2D_HPP

#include <cmath> // std::abs
#include <functional> // std::hash
#include <iostream> // ostream
#include <string>

#include "sfz/math/Vector.hpp"

namespace sfz {

/** @brief A POD class representing a 2D AABB. */
struct AABB2D final {
	
	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	vec2 min, max;

	// Constructors and destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	AABB2D() noexcept = default;
	AABB2D(const AABB2D&) noexcept = default;
	AABB2D& operator= (const AABB2D&) noexcept = default;
	~AABB2D() noexcept = default;

	inline AABB2D(vec2 centerPos, vec2 dimensions) noexcept;
	inline AABB2D(float centerX, float centerY, float width, float height) noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline size_t hash() const noexcept;
	inline std::string to_string() const noexcept;

	// Public getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline vec2 position() const noexcept { return (min + max) / 2.0f; }
	inline float x() const noexcept { return (min.x + max.x) / 2.0f; }
	inline float y() const noexcept { return (min.y + max.y) / 2.0f; }
	inline vec2 dimensions() const noexcept { return max - min; }
	inline float width() const noexcept { return max.x - min.x; }
	inline float height() const noexcept { return max.y - min.y; }

	// Comparison operators
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline bool operator== (const AABB2D& other) const noexcept;
	inline bool operator!= (const AABB2D& other) const noexcept;
};

// Standard typedefs
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

using Rectangle = AABB2D;

// Free (non-member) operators
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline std::ostream& operator<< (std::ostream& ostream, const AABB2D& aabb) noexcept;

} // namespace sfz

// Specializations of standard library for sfz::AABB2D
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
namespace std {

template<>
struct hash<sfz::AABB2D> {
	inline size_t operator() (const sfz::AABB2D& aabb) const noexcept;
};

} // namespace std
#include "sfz/geometry/AABB2D.inl"
#endif