#pragma once
#ifndef SFZ_MATH_MATH_HELPERS_HPP
#define SFZ_MATH_MATH_HELPERS_HPP

#include <algorithm>
#include <cmath>
#include <type_traits>

#include "sfz/math/Vector.hpp"
#include "sfz/math/Matrix.hpp"

namespace sfz {

using std::size_t;

template<typename T>
T defaultEpsilon() { return T(0.0001); }

template<typename T>
bool approxEqual(T lhs, T rhs, T epsilon) noexcept;

template<typename T>
bool approxEqual(T lhs, T rhs) noexcept;

template<typename T, size_t N>
bool approxEqual(const Vector<T,N>& lhs, const Vector<T,N>& rhs, T epsilon) noexcept;

template<typename T, size_t N>
bool approxEqual(const Vector<T,N>& lhs, const Vector<T,N>& rhs) noexcept;

template<typename T, size_t M, size_t N>
bool approxEqual(const Matrix<T,M,N>& lhs, const Matrix<T,M,N>& rhs, T epsilon) noexcept;

template<typename T, size_t M, size_t N>
bool approxEqual(const Matrix<T,M,N>& lhs, const Matrix<T,M,N>& rhs) noexcept;

/**
 * @brief Lerp function, v0 when t == 0 and v1 when t == 1.
 * @see http://en.wikipedia.org/wiki/Lerp_%28computing%29
 */
template<typename ArgT, typename FloatT>
ArgT lerp(ArgT v0, ArgT v1, FloatT t) noexcept;

template<typename T>
T clamp(T value, T minValue, T maxValue);

} // namespace sfz
#include "sfz/math/MathHelpers.inl"
#endif