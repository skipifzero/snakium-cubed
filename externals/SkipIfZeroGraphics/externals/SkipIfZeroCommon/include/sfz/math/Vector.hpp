#pragma once
#ifndef SFZ_MATH_VECTOR_HPP
#define SFZ_MATH_VECTOR_HPP

#include <cstddef> // std::size_t
#include <cstdint> // std::int32_t
#include <functional> // std::hash
#include <cmath> // std::sqrt
#include <string>
#include <iostream> // std::ostream

#include "sfz/Assert.hpp"
#include "sfz/math/MathConstants.hpp"

/**
 * @brief A mathematical vector POD class that imitates a built-in primitive.
 *
 * Typedefs are provided for float vectors (vec2, vec3 and vec4) and (32-bit signed) integer
 * vectors (vec2i, vec3i, vec4i). Note that for integers some operations, such as as calculating
 * the length, may give unexpected results due to truncation or overflow.
 *
 * 2, 3 and 4 dimensional vectors are specialized to have more constructors and ways of accessing
 * data. For example, you can construct a vec3 with 3 floats (vec3(x, y, z)), or with a vec2 and a 
 * float (vec3(vec2(x,y), z) or vec3(x, vec2(y, z))). To access the x value of a vec3 v you can
 * write v[0], v.elements[0] or v.x, you can also access two adjacent elements as a vector by
 * writing v.xy or v.yz.
 *
 * Satisfies the conditions of std::is_pod, std::is_trivial and std::is_standard_layout if used
 * with standard primitives.
 *
 * @author Peter Hillerström <peter@hstroem.se>
 */

namespace sfz {

using std::size_t;
using std::int32_t;

// Vector struct declaration
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T, size_t N>
struct Vector final {

	T elements[N];

	constexpr Vector() noexcept = default;
	constexpr Vector(const Vector<T,N>&) noexcept = default;
	Vector<T,N>& operator= (const Vector<T,N>&) noexcept = default;
	~Vector() noexcept = default;

	explicit Vector(const T* arrayPtr) noexcept;

	T& operator[] (const size_t index) noexcept;
	T operator[] (const size_t index) const noexcept;
};

template<typename T>
struct Vector<T,2> final {
	union {
		T elements[2];
		struct { T x, y; };
	};	

	constexpr Vector() noexcept = default;
	constexpr Vector(const Vector<T,2>&) noexcept = default;
	Vector<T,2>& operator= (const Vector<T,2>&) noexcept = default;
	~Vector() noexcept = default;

	explicit Vector(const T* arrayPtr) noexcept;
	explicit Vector(T value) noexcept;
	Vector(T x, T y) noexcept;

	T& operator[] (const size_t index) noexcept;
	T operator[] (const size_t index) const noexcept;
};

template<typename T>
struct Vector<T,3> final {
	union {
		T elements[3];
		struct { T x, y, z; };
		struct { Vector<T,2> xy; };
		struct { T xAlias; Vector<T,2> yz; };
	};

	constexpr Vector() noexcept = default;
	constexpr Vector(const Vector<T,3>&) noexcept = default;
	Vector<T,3>& operator= (const Vector<T,3>&) noexcept = default;
	~Vector() noexcept = default;

	explicit Vector(const T* arrayPtr) noexcept;
	explicit Vector(T value) noexcept;
	Vector(T x, T y, T z) noexcept;
	Vector(Vector<T,2> xy, T z) noexcept;
	Vector(T x, Vector<T,2> yz) noexcept;

	T& operator[] (const size_t index) noexcept;
	T operator[] (const size_t index) const noexcept;
};

template<typename T>
struct Vector<T,4> final {
	union {
		T elements[4];
		struct { T x, y, z, w; };
		struct { Vector<T,3> xyz; };
		struct { T xAlias1; Vector<T,3> yzw; };
		struct { Vector<T,2> xy, zw; };
		struct { T xAlias2; Vector<T,2> yz; };
	};

	constexpr Vector() noexcept = default;
	constexpr Vector(const Vector<T,4>&) noexcept = default;
	Vector<T,4>& operator= (const Vector<T,4>&) noexcept = default;
	~Vector() noexcept = default;

	explicit Vector(const T* arrayPtr) noexcept;
	explicit Vector(T value) noexcept;
	Vector(T x, T y, T z, T w) noexcept;
	Vector(Vector<T,3> xyz, T w) noexcept;
	Vector(T x, Vector<T,3> yzw) noexcept;
	Vector(Vector<T,2> xy, Vector<T,2> zw) noexcept;
	Vector(Vector<T,2> xy, T z, T w) noexcept;
	Vector(T x, Vector<T,2> yz, T w) noexcept;
	Vector(T x, T y, Vector<T,2> zw) noexcept;

	T& operator[] (const size_t index) noexcept;
	T operator[] (const size_t index) const noexcept;
};

using vec2 = Vector<float,2>;
using vec3 = Vector<float,3>;
using vec4 = Vector<float,4>;

using vec2i = Vector<int32_t,2>;
using vec3i = Vector<int32_t,3>;
using vec4i = Vector<int32_t,4>;

// Vector constants
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T = float>
Vector<T,3> UNIT_X() noexcept;

template<typename T = float>
Vector<T,3> UNIT_Y() noexcept;

template<typename T = float>
Vector<T,3> UNIT_Z() noexcept;

// Vector functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/** @brief Calculates length of the vector */
template<typename T, size_t N>
T length(const Vector<T,N>& vector) noexcept;

/** @brief Calculates squared length of vector */
template<typename T, size_t N>
T squaredLength(const Vector<T,N>& vector) noexcept;

/**
 * @brief Normalizes vector
 * @sfz_assert_debug length of vector is not zero
 */
template<typename T, size_t N>
Vector<T,N> normalize(const Vector<T,N>& vector) noexcept;

/** @brief Calculates the dot product of two vectors */
template<typename T, size_t N>
T dot(const Vector<T,N>& left, const Vector<T,N>& right) noexcept;

/** @brief Calculates the cross product of two vectors */
template<typename T>
constexpr Vector<T,3> cross(const Vector<T,3>& left, const Vector<T,3>& right) noexcept;

/** @brief Calculates the sum of all the elements in the vector */
template<typename T, size_t N>
T sum(const Vector<T,N>& vector) noexcept;

/**
 * @brief Calculates the positive angle (in radians) between two vectors
 * Range: [0, Pi])
 * @sfz_assert_debug norm of both vectors != 0
 */
template<typename T, size_t N>
T angle(const Vector<T,N>& left, const Vector<T,N>& right) noexcept;

/**
 * @brief Calculates the positive angle (in radians) between the vector and the x-axis
 * Range: [0, 2*Pi).
 * @sfz_assert_debug norm of vector != 0
 */
template<typename T>
T angle(Vector<T,2> vector) noexcept;

/** @brief Rotates a 2-dimensional vector with the specified angle (in radians) around origo */
template<typename T>
Vector<T,2> rotate(Vector<T,2> vector, T angleRadians) noexcept;

/** @brief Hashes the vector */
template<typename T, size_t N>
size_t hash(const Vector<T,N>& vector) noexcept;

/** @brief Creates string representation of the vector */
template<typename T, size_t N>
std::string to_string(const Vector<T,N>& vector) noexcept;

// Operators (arithmetic & assignment)
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T, size_t N>
Vector<T,N>& operator+= (Vector<T,N>& left, const Vector<T,N>& right) noexcept;

template<typename T, size_t N>
Vector<T,N>& operator-= (Vector<T,N>& left, const Vector<T,N>& right) noexcept;

template<typename T, size_t N>
Vector<T,N>& operator*= (Vector<T,N>& left, T right) noexcept;

/** @brief Element-wise multiplication assignment */
template<typename T, size_t N>
Vector<T,N>& operator*= (Vector<T,N>& left, const Vector<T,N>& right) noexcept;

/** @sfz_assert_debug rhs element != 0 */
template<typename T, size_t N>
Vector<T,N>& operator/= (Vector<T,N>& left, T right) noexcept;

// Operators (arithmetic)
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T, size_t N>
Vector<T,N> operator+ (const Vector<T,N>& left, const Vector<T,N>& right) noexcept;

template<typename T, size_t N>
Vector<T,N> operator- (const Vector<T,N>& left, const Vector<T,N>& right) noexcept;

template<typename T, size_t N>
Vector<T,N> operator- (const Vector<T,N>& vector) noexcept;

template<typename T, size_t N>
Vector<T,N> operator* (const Vector<T,N>& left, T right) noexcept;

/** @brief Element-wise multiplication of two vectors */
template<typename T, size_t N>
Vector<T,N> operator* (const Vector<T,N>& left, const Vector<T,N>& right) noexcept;

template<typename T, size_t N>
Vector<T,N> operator* (T left, const Vector<T,N>& right) noexcept;

/** @sfz_assert_debug rhs element != 0 */
template<typename T, size_t N>
Vector<T,N> operator/ (const Vector<T,N>& left, T right) noexcept;

// Operators (comparison)
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T, size_t N>
bool operator== (const Vector<T,N>& left, const Vector<T,N>& right) noexcept;

template<typename T, size_t N>
bool operator!= (const Vector<T,N>& left, const Vector<T,N>& right) noexcept;

// Operators (other)
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T, size_t N>
std::ostream& operator<< (std::ostream& ostream, const Vector<T,N>& vector) noexcept;

// Standard iterator functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T, size_t N>
T* begin(Vector<T,N>& vector) noexcept;

template<typename T, size_t N>
const T* begin(const Vector<T,N>& vector) noexcept;

template<typename T, size_t N>
const T* cbegin(const Vector<T,N>& vector) noexcept;

template<typename T, size_t N>
T* end(Vector<T,N>& vector) noexcept;

template<typename T, size_t N>
const T* end(const Vector<T,N>& vector) noexcept;

template<typename T, size_t N>
const T* cend(const Vector<T,N>& vector) noexcept;

} // namespace sfz

// Specializations of standard library for sfz::Vector
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

namespace std {

template<typename T, size_t N>
struct hash<sfz::Vector<T,N>> {
	size_t operator() (const sfz::Vector<T,N>& vector) const noexcept;
};

} // namespace std
#include "Vector.inl"
#endif