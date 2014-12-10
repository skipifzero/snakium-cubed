namespace sfz {

// Rotation matrices
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
Matrix<T,4,4> xRotationMatrix(T angleRads) noexcept
{
	using std::cos;
	using std::sin;
	return Matrix<T,4,4>{{1, 0, 0, 0},
	                     {0, cos(angleRads), -sin(angleRads), 0},
	                     {0, sin(angleRads), cos(angleRads), 0},
	                     {0, 0, 0, 1}};
}

template<typename T>
Matrix<T,4,4> yRotationMatrix(T angleRads) noexcept
{
	using std::cos;
	using std::sin;
	return Matrix<T,4,4>{{cos(angleRads), 0, sin(angleRads), 0},
	                     {0, 1, 0, 0},
	                     {-sin(angleRads), 0, cos(angleRads), 0},
	                     {0, 0, 0, 1}};
}

template<typename T>
Matrix<T,4,4> zRotationMatrix(T angleRads) noexcept
{
	using std::cos;
	using std::sin;
	return Matrix<T,4,4>{{cos(angleRads), -sin(angleRads), 0, 0},
	                     {sin(angleRads), cos(angleRads), 0, 0},
	                     {0, 0, 1, 0},
	                     {0, 0, 0, 1}};
}

// Transformation matrices
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
Matrix<T,4,4> identityMatrix() noexcept
{
	return Matrix<T,4,4>{{1, 0, 0, 0},
	                     {0, 1, 0, 0},
	                     {0, 0, 1, 0},
	                     {0, 0, 0, 1}};
}

template<typename T>
Matrix<T,4,4> scalingMatrix(T scaleFactor) noexcept
{
	return Matrix<T,4,4>{{scaleFactor, 0, 0, 0},
	                     {0, scaleFactor, 0, 0},
	                     {0, 0, scaleFactor, 0},
	                     {0, 0, 0, 1}};
}

template<typename T>
Matrix<T,4,4> scalingMatrix(T scaleX, T scaleY, T scaleZ) noexcept
{
	return Matrix<T,4,4>{{scaleX, 0, 0, 0},
	                     {0, scaleY, 0, 0},
	                     {0, 0, scaleZ, 0},
	                     {0, 0, 0, 1}};
}

template<typename T>
Matrix<T,4,4> translationMatrix(T deltaX, T deltaY, T deltaZ) noexcept
{
	return Matrix<T,4,4>{{1, 0, 0, deltaX},
	                     {0, 1, 0, deltaY},
	                     {0, 0, 1, deltaZ},
	                     {0, 0, 0, 1}};
}

template<typename T>
Matrix<T,4,4> translationMatrix(const Vector<T,3>& delta) noexcept
{
	return translationMatrix(delta[0], delta[1], delta[2]);
}

// Projection matrices
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
Matrix<T,4,4> glOrthogonalProjectionMatrix(T left, T bottom, T near,
                                           T right, T top, T far) noexcept
{
	return Matrix<T,4,4>{{2/(right-left), 0, 0, -((right+left)/(right-left))},
	                     {0, 2/(top-bottom), 0, -((top+bottom)/(top-bottom))},
	                     {0, 0, -2/(far-near), -(far+near)/(far-near)},
	                     {0, 0, 0, 1}};
}

template<typename T>
Matrix<T,4,4> glOrthogonalProjectionMatrix(const sfz::Vector<T,3>& leftBottomNear,
                                           const sfz::Vector<T,3>& rightTopFar) noexcept
{
	return glOrthogonalProjectionMatrix(leftBottomNear[0], leftBottomNear[1], leftBottomNear[2],
	                                    rightTopFar[0], rightTopFar[1], rightTopFar[2]);
}

template<typename T>
Matrix<T,4,4> glPerspectiveProjectionMatrix(T left, T bottom, T zNear,
                                            T right, T top, T zFar) noexcept
{
	T zNear2 = 2*zNear;
	T rightMLeft = right-left;
	T topMBottom = top-bottom;
	T zFarMzNear = zFar-zNear;
	return Matrix<T,4,4>{{zNear2/rightMLeft, 0, (right+left)/rightMLeft, 0},
	                     {0, zNear2/topMBottom, (top+bottom)/topMBottom, 0},
	                     {0, 0, -(zFar+zNear)/zFarMzNear, -(zNear2*zFar)/zFarMzNear},
	                     {0, 0, -1, 0}};
}

inline Matrix<float,4,4> glPerspectiveProjectionMatrix(float yFovDeg, float aspectRatio,
                                                       float zNear, float zFar) noexcept
{
	assert(0 < zNear);
	assert(zNear < zFar);
	float yMax = zNear * tanf(yFovDeg * (g_PI_FLOAT/360.f));
	float xMax = yMax * aspectRatio;
	return glPerspectiveProjectionMatrix(-xMax, -yMax, zNear, xMax, yMax, zFar);
}

// View matrices
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
Matrix<T,4,4> lookAt(const Vector<T,3>& cameraPosition, const Vector<T,3> cameraTarget,
                     const Vector<T,3> upVector) noexcept
{
	// Inspired by gluLookAt().
	Vector<T,3> normalizedDir = (cameraTarget - cameraPosition).normalize();
	Vector<T,3> normalizedUpVec = upVector.normalize();
	Vector<T,3> s = cross(normalizedDir, normalizedUpVec).normalize();
	Vector<T,3> u = cross(s.normalize(), normalizedDir).normalize();
	return Matrix<T,4,4>{{s[0], s[1], s[2], 0},
	                     {u[0], u[1], u[2], 0},
	                     {-normalizedDir[0], -normalizedDir[1], -normalizedDir[2], 0},
	                     {0, 0, 0, 1}}

	                     * translationMatrix(-cameraPosition);
}

} // namespace sfz