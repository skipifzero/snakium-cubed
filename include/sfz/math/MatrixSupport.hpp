#pragma once
#ifndef SFZ_MATH_MATRIX_SUPPORT_HPP
#define SFZ_MATH_MATRIX_SUPPORT_HPP

#include <cmath>
#include "sfz/math/Matrix.hpp"

namespace sfz {

/**
 * This header includes support functions for the Matrix class.
 *
 * @author Peter Hillerström
 */

// Rotation matrices
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
Matrix<T,4,4> xRotationMatrix(T angleRads) noexcept;

template<typename T>
Matrix<T,4,4> yRotationMatrix(T angleRads) noexcept;

template<typename T>
Matrix<T,4,4> zRotationMatrix(T angleRads) noexcept;

/*template<typename T>
Matrix<T,4,4> generalRotationMatrix(const Vector<T,3>& point, const Vector<T,3>& axis,
                                    T angleRads) noexcept;
*/

// Transformation matrices
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
Matrix<T,4,4> identityMatrix() noexcept;

template<typename T>
Matrix<T,4,4> scalingMatrix(T scaleFactor) noexcept;

template<typename T>
Matrix<T,4,4> scalingMatrix(T scaleX, T scaleY, T scaleZ) noexcept;

template<typename T>
Matrix<T,4,4> translationMatrix(T deltaX, T deltaY, T deltaZ) noexcept;

} // namespace sfz

#include "sfz/math/MatrixSupport.inl"
#endif