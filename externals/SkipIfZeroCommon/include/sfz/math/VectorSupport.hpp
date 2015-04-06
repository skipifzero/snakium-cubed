#pragma once
#ifndef SFZ_MATH_VECTOR_SUPPORT_HPP
#define SFZ_MATH_VECTOR_SUPPORT_HPP

#include <cmath>

#include "sfz/Assert.hpp"
#include "sfz/math/Vector.hpp"
#include "sfz/MSVC12HackON.hpp"

namespace sfz {

/**
 * This header includes support functions for the Vector class.
 *
 * @author Peter Hillerström
 */

/**
 * @brief Calculates the cross product of the vectors.
 * @return the resulting vector
 */
template<typename T>
Vector<T,3> cross(const Vector<T,3>& vectorA, const Vector<T,3>& vectorB) noexcept;

/**
 * @brief Calculates the positive angle between two vectors.
 * The angle will be in range [0, Pi] and will always be the smallest possible angle between the
 * vectors.
 * @sfz_assert_debug norm of vectorA or B != 0
 * @return the angle between the two vectors
 */
template<typename T, size_t N>
T angle(const Vector<T,N>& vectorA, const Vector<T,N>& vectorB) noexcept;

/**
 * @brief Calculates the positive angle between the specified vector and the x-axis in radians.
 * The angle will be in the range [0, 2*Pi).
 * @sfz_assert_debug norm of vector != 0
 * @return the angle between the vector and the x-axis
 */
template<typename T>
T angle(const Vector<T,2>& vector) noexcept;

/**
 * @brief Rotates a 2-dimensional vector with the specified angle around origo.
 * @return the resulting vector
 */
template<typename T>
Vector<T,2> rotate(const Vector<T,2>& vector, const T angle) noexcept;

/**
 * @brief Projects a vector onto another vector.
 * Makes a scalar projection of a vector onto the specifed target vector. The resulting
 * vector will be equal to the target vector times a scalar constant. If the target vector is
 * 0 the 0 vector will be returned.
 * @param toProject the vector to project
 * @param target the vector to project onto
 * @return the resulting projection in vector form
 */
template<typename T, size_t N>
Vector<T,N> projectOnto(const Vector<T,N>& toProject, const Vector<T,N>& target) noexcept;

} // namespace sfz

#include "sfz/MSVC12HackOFF.hpp"
#include "sfz/math/VectorSupport.inl"
#endif