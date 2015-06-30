#pragma once
#ifndef SFZ_GEOMETRY_PLANE_HPP
#define SFZ_GEOMETRY_PLANE_HPP

#include <string>
#include <iostream> // std::ostream
#include <functional> // std::hash

#include "sfz/Assert.hpp"
#include "sfz/math/Vector.hpp"
#include "sfz/math/MathHelpers.hpp"

namespace sfz {

/**
 * @brief Class representing a Plane.
 *
 * Mathematical definition (plane normal = n, position on plane = p, position to test = x):
 * f(x) = dot(n, x - p) = dot(n, q) - d
 * d = dot(n, p)
 * 
 * @author Peter Hillerström
 */
class Plane final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline Plane() noexcept = default;

	/** @brief dot(normal, x) - d = 0 */
	inline Plane(const vec3& normal, float d) noexcept;
	/** @brief dot(normal, x - position) = 0 */
	inline Plane(const vec3& normal, const vec3& position) noexcept;

	// Public member functions
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	/** @brief Returns the signed distance to the plane. Positive if above, negative if below. */
	inline float signedDistance(const vec3& point) const noexcept;
	inline vec3 closestPoint(const vec3& point) const noexcept;
	inline size_t hash() const noexcept;
	inline std::string to_string() const noexcept;

	// Public getters/setters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline const vec3& normal() const noexcept { return mNormal; }
	inline float d() const noexcept { return mD; }

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	vec3 mNormal;
	float mD;
};

// Non-member operators
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline std::ostream& operator<< (std::ostream& ostream, const Plane& plane) noexcept;

} // namespace sfz

// Specializations of standard library for sfz::Plane
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

namespace std {

template<>
struct hash<sfz::Plane> {
	inline size_t operator() (const sfz::Plane& plane) const noexcept;
};

} // namespace std

#include "sfz/geometry/Plane.inl"
#endif