#pragma once
#ifndef SFZ_MATH_MATH_HELPERS_HPP
#define SFZ_MATH_MATH_HELPERS_HPP

#include <cmath>
#include <type_traits>

#include "sfz/math/Vector.hpp"
#include "sfz/math/Matrix.hpp"

#include "sfz/MSVC12HackON.hpp"

namespace sfz {

using std::size_t;

template<typename T>
T defaultEpsilon() { return static_cast<T>(0.00001); }

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

} // namespace sfz

#include "sfz/MSVC12HackOFF.hpp"
#include "sfz/math/MathHelpers.inl"
#endif