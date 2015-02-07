#pragma once
#ifndef SFZ_MATH_MATRIX_SUPPORT_HPP
#define SFZ_MATH_MATRIX_SUPPORT_HPP

#include <cmath>
#include "sfz/math/Matrix.hpp"
#include "sfz/MSVC12HackON.hpp"

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

template<typename T>
Matrix<T,4,4> rotationMatrix(const sfz::Vector<T,3>& axis, T angleRads) noexcept;

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

template<typename T>
Matrix<T,4,4> translationMatrix(const Vector<T,3>& delta) noexcept;

// Projection matrices
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
Matrix<T,4,4> glOrthogonalProjectionMatrix(T left, T bottom, T near,
                                           T right, T top, T far) noexcept;

template<typename T>
Matrix<T,4,4> glOrthogonalProjectionMatrix(const sfz::Vector<T,3>& leftBottomNear,
                                           const sfz::Vector<T,3>& rightTopFar) noexcept;

template<typename T>
Matrix<T,4,4> glPerspectiveProjectionMatrix(T left, T bottom, T zNear,
                                            T right, T top, T zFar) noexcept;

/**
 * @brief Creates a perspective matrix for use with OpenGL.
 * @assert 0 < zNear < zFar
 * @param yFovDeg the vertical fov in degrees
 * @param aspectRatio the width / height ratio of the frustrum
 * @param zNear the near plane
 * @param zFar the far plane
 */
inline Matrix<float,4,4> glPerspectiveProjectionMatrix(float yFovDeg, float aspectRatio,
                                                       float zNear, float zFar) noexcept;

// View matrices
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
Matrix<T,4,4> lookAt(const Vector<T,3>& cameraPosition, const Vector<T,3> cameraTarget,
                     const Vector<T,3> upVector) noexcept;

// Transform helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
Vector<T,3> translation(const Matrix<T,4,4>& transform) noexcept;

template<typename T>
void translation(Matrix<T,4,4>& transform, const Vector<T,3>& translation) noexcept;

template<typename T>
Vector<T,3> scaling(const Matrix<T,4,4>& transform) noexcept;

template<typename T>
void scaling(Matrix<T,4,4>& transform, const Vector<T,3>& scaling) noexcept;

template<typename T>
Vector<T,3> forward(const Matrix<T,4,4>& transform) noexcept;

template<typename T>
void forward(Matrix<T,4,4>& transform, const Vector<T,3>& forward) noexcept;

template<typename T>
Vector<T,3> backward(const Matrix<T,4,4>& transform) noexcept;

template<typename T>
void backward(Matrix<T,4,4>& transform, const Vector<T,3>& backward) noexcept;

template<typename T>
Vector<T,3> up(const Matrix<T,4,4>& transform) noexcept;

template<typename T>
void up(Matrix<T,4,4>& transform, const Vector<T,3>& up) noexcept;

template<typename T>
Vector<T,3> down(const Matrix<T,4,4>& transform) noexcept;

template<typename T>
void down(Matrix<T,4,4>& transform, const Vector<T,3>& down) noexcept;

template<typename T>
Vector<T,3> right(const Matrix<T,4,4>& transform) noexcept;

template<typename T>
void right(Matrix<T,4,4>& transform, const Vector<T,3>& right) noexcept;

template<typename T>
Vector<T,3> left(const Matrix<T,4,4>& transform) noexcept;

template<typename T>
void left(Matrix<T,4,4>& transform, const Vector<T,3>& left) noexcept;

} // namespace sfz

#include "sfz/MSVC12HackOFF.hpp"
#include "sfz/math/MatrixSupport.inl"
#endif