#include "sfz/MSVC12HackON.hpp"

namespace sfz {

// Resizing helpers
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
Matrix<T,3,3> mat3(const Matrix<T,4,4>& m) noexcept
{
	return Matrix<T,3,3>{{m.at(0,0), m.at(0,1), m.at(0,2)},
	                     {m.at(1,0), m.at(1,1), m.at(1,2)},
	                     {m.at(2,0), m.at(2,1), m.at(2,2)}};
}

template<typename T>
Matrix<T,4,4> mat4(const Matrix<T,3,3>& m) noexcept
{
	return Matrix<T,4,4>{{m.at(0,0), m.at(0,1), m.at(0,2), 0},
	                     {m.at(1,0), m.at(1,1), m.at(1,2), 0},
	                     {m.at(2,0), m.at(2,1), m.at(2,2), 0},
	                     {0, 0, 0, 1}};
}

// Transforming 3D vector helpers
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
Vector<T,3> transformPoint(const Matrix<T,4,4>& m, const Vector<T,3>& p) noexcept
{
	Vector<T,4> v4{p[0], p[1], p[2], 1};
	v4 = m * v4;
	v4 = v4 / v4[3];
	return Vector<T,3>{v4[0], v4[1], v4[2]};
}

template<typename T>
Vector<T,3> transformDir(const Matrix<T,4,4>& m, const Vector<T,3>& d) noexcept
{
	Vector<T,4> v4{d[0], d[1], d[2], 0};
	v4 = m * v4;
	return Vector<T,3>{v4[0], v4[1], v4[2]};
}

// Common specialized operations
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
T determinant(const Matrix<T,2,2>& m) noexcept
{
	return m.at(0,0)*m.at(1,1) - m.at(0,1)*m.at(1,0);
}

template<typename T>
T determinant(const Matrix<T,3,3>& m) noexcept
{
	return m.at(0, 0) * m.at(1, 1) * m.at(2, 2)
	     + m.at(0, 1) * m.at(1, 2) * m.at(2, 0)
	     + m.at(0, 2) * m.at(1, 0) * m.at(2, 1)
	     - m.at(0, 2) * m.at(1, 1) * m.at(2, 0)
	     - m.at(0, 1) * m.at(1, 0) * m.at(2, 2)
	     - m.at(0, 0) * m.at(1, 2) * m.at(2, 1);
}

template<typename T>
Matrix<T,2,2> inverse(const Matrix<T,2,2>& m) noexcept
{
	const T det = determinant(m);
	if (det == 0) return Matrix<T,2,2>{{0,0},{0,0}};

	Matrix<T,2,2> temp{{m.at(1, 1), -m.at(0, 1)},
	                  {-m.at(1,0), m.at(0,0)}};

	return (1/det) * temp;
}

template<typename T>
Matrix<T,3,3> inverse(const Matrix<T,3,3>& m) noexcept
{
	const T det = determinant(m);
	if (det == 0) return Matrix<T,3,3>{{0,0,0},{0,0,0},{0,0,0}};

	const T a = m.at(0,0);
	const T b = m.at(0,1); 
	const T c = m.at(0,2);
	const T d = m.at(1,0);
	const T e = m.at(1,1); 
	const T f = m.at(1,2);
	const T g = m.at(2,0);
	const T h = m.at(2,1); 
	const T i = m.at(2,2);

	
	const T A = (e*i - f*h);
	const T B = -(d*i - f*g);
	const T C = (d*h - e*g);
	const T D = -(b*i - c*h);
	const T E = (a*i - c*g);
	const T F = -(a*h - b*g);
	const T G = (b*f - c*e);
	const T H = -(a*f - c*d);
	const T I = (a*e - b*d);

	Matrix<T,3,3> temp{{A, D, G}, {B, E, H}, {C, F, I}};
	return (1/det) * temp;
}

// Rotation matrices
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
Matrix<T,3,3> xRotationMatrix3(T angleRads) noexcept
{
	using std::cos;
	using std::sin;
	return Matrix<T,3,3>{{1, 0, 0},
	                     {0, cos(angleRads), -sin(angleRads)},
	                     {0, sin(angleRads), cos(angleRads)}};
}

template<typename T>
Matrix<T,4,4> xRotationMatrix4(T angleRads) noexcept
{
	using std::cos;
	using std::sin;
	return Matrix<T,4,4>{{1, 0, 0, 0},
	                     {0, cos(angleRads), -sin(angleRads), 0},
	                     {0, sin(angleRads), cos(angleRads), 0},
	                     {0, 0, 0, 1}};
}

template<typename T>
Matrix<T,3,3> yRotationMatrix3(T angleRads) noexcept
{
	using std::cos;
	using std::sin;
	return Matrix<T,3,3>{{cos(angleRads), 0, sin(angleRads)},
	                     {0, 1, 0},
	                     {-sin(angleRads), 0, cos(angleRads)}};
}

template<typename T>
Matrix<T,4,4> yRotationMatrix4(T angleRads) noexcept
{
	using std::cos;
	using std::sin;
	return Matrix<T,4,4>{{cos(angleRads), 0, sin(angleRads), 0},
	                     {0, 1, 0, 0},
	                     {-sin(angleRads), 0, cos(angleRads), 0},
	                     {0, 0, 0, 1}};
}

template<typename T>
Matrix<T,3,3> zRotationMatrix3(T angleRads) noexcept
{
	using std::cos;
	using std::sin;
	return Matrix<T,3,3>{{cos(angleRads), -sin(angleRads), 0},
	                     {sin(angleRads), cos(angleRads), 0},
	                     {0, 0, 1}};
}

template<typename T>
Matrix<T,4,4> zRotationMatrix4(T angleRads) noexcept
{
	using std::cos;
	using std::sin;
	return Matrix<T,4,4>{{cos(angleRads), -sin(angleRads), 0, 0},
	                     {sin(angleRads), cos(angleRads), 0, 0},
	                     {0, 0, 1, 0},
	                     {0, 0, 0, 1}};
}

template<typename T>
Matrix<T,3,3> rotationMatrix3(const sfz::Vector<T,3>& axis, T angleRads) noexcept
{
	using std::cos;
	using std::sin;
	sfz::Vector<T,3> r = axis.normalize();
	T x = r[0];
	T y = r[1];
	T z = r[2];
	T c = cos(angleRads);
	T s = sin(angleRads);
	T cm1 = static_cast<T>(1) - c;
	// Matrix by Goldman, page 71 of Real-Time Rendering.
	return Matrix<T,3,3>{{c + cm1*x*x, cm1*x*y - z*s, cm1*x*z + y*s},
	                     {cm1*x*y + z*s, c + cm1*y*y, cm1*y*z - x*s},
	                     {cm1*x*z - y*s, cm1*y*z + x*s, c + cm1*z*z}};
}

template<typename T>
Matrix<T,4,4> rotationMatrix4(const sfz::Vector<T,3>& axis, T angleRads) noexcept
{
	using std::cos;
	using std::sin;
	sfz::Vector<T,3> r = axis.normalize();
	T x = r[0];
	T y = r[1];
	T z = r[2];
	T c = cos(angleRads);
	T s = sin(angleRads);
	T cm1 = static_cast<T>(1) - c;
	// Matrix by Goldman, page 71 of Real-Time Rendering.
	return Matrix<T,4,4>{{c + cm1*x*x, cm1*x*y - z*s, cm1*x*z + y*s, 0},
	                     {cm1*x*y + z*s, c + cm1*y*y, cm1*y*z - x*s, 0},
	                     {cm1*x*z - y*s, cm1*y*z + x*s, c + cm1*z*z, 0},
	                     {0, 0, 0, 1}};
}

// Transformation matrices
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
Matrix<T,3,3> identityMatrix3() noexcept
{
	return Matrix<T,3,3>{{1, 0, 0},
	                     {0, 1, 0},
	                     {0, 0, 1}};
}

template<typename T>
Matrix<T,4,4> identityMatrix4() noexcept
{
	return Matrix<T,4,4>{{1, 0, 0, 0},
	                     {0, 1, 0, 0},
	                     {0, 0, 1, 0},
	                     {0, 0, 0, 1}};
}

template<typename T>
Matrix<T,3,3> scalingMatrix3(T scaleFactor) noexcept
{
	return Matrix<T,3,3>{{scaleFactor, 0, 0},
	                     {0, scaleFactor, 0},
	                     {0, 0, scaleFactor}};
}

template<typename T>
Matrix<T,4,4> scalingMatrix4(T scaleFactor) noexcept
{
	return Matrix<T,4,4>{{scaleFactor, 0, 0, 0},
	                     {0, scaleFactor, 0, 0},
	                     {0, 0, scaleFactor, 0},
	                     {0, 0, 0, 1}};
}

template<typename T>
Matrix<T,3,3> scalingMatrix3(T scaleX, T scaleY, T scaleZ) noexcept
{
	return Matrix<T,3,3>{{scaleX, 0, 0},
	                     {0, scaleY, 0},
	                     {0, 0, scaleZ}};
}

template<typename T>
Matrix<T,4,4> scalingMatrix4(T scaleX, T scaleY, T scaleZ) noexcept
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
Matrix<T,4,4> glOrthogonalProjectionMatrix(T left, T bottom, T zNear,
                                           T right, T top, T zFar) noexcept
{
	// Note: near is renamed to zNear to fix a bug when compiling on MSVC. I have no idea. =/
	return Matrix<T,4,4>{{2/(right-left), 0, 0, -((right+left)/(right-left))},
	                     {0, 2/(top-bottom), 0, -((top+bottom)/(top-bottom))},
	                     {0, 0, -2/(zFar-zNear), -(zFar+zNear)/(zFar-zNear)},
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
	sfz_assert_debug(0 < zNear);
	sfz_assert_debug(zNear < zFar);
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

// Transform helper functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T>
Vector<T,3> translation(const Matrix<T,4,4>& transform) noexcept
{
	Vector<T,3> temp;
	temp[0] = transform.at(0, 3);
	temp[1] = transform.at(1, 3);
	temp[2] = transform.at(2, 3);
	return temp;
}

template<typename T>
void translation(Matrix<T,4,4>& transform, const Vector<T,3>& translation) noexcept
{
	transform.set(0, 3, translation[0]);
	transform.set(1, 3, translation[1]);
	transform.set(2, 3, translation[2]);
}

template<typename T>
Vector<T,3> scaling(const Matrix<T,4,4>& transform) noexcept
{
	Vector<T,3> temp;
	temp[0] = transform.at(0, 0);
	temp[1] = transform.at(1, 1);
	temp[2] = transform.at(2, 2);
	return temp;
}

template<typename T>
void scaling(Matrix<T,4,4>& transform, const Vector<T,3>& scaling) noexcept
{
	transform.set(0, 0, scaling[0]);
	transform.set(1, 1, scaling[1]);
	transform.set(2, 2, scaling[2]);
}

template<typename T>
Vector<T,3> forward(const Matrix<T,4,4>& transform) noexcept
{
	Vector<T,3> temp;
	temp[0] = transform.at(0, 2);
	temp[1] = transform.at(1, 2);
	temp[2] = transform.at(2, 2);
	return temp;
}

template<typename T>
void forward(Matrix<T,4,4>& transform, const Vector<T,3>& forward) noexcept
{
	transform.set(0, 2, forward[0]);
	transform.set(1, 2, forward[1]);
	transform.set(2, 2, forward[2]);
}

template<typename T>
Vector<T,3> backward(const Matrix<T,4,4>& transform) noexcept
{
	return -forward(transform);
}

template<typename T>
void backward(Matrix<T,4,4>& transform, const Vector<T,3>& backward) noexcept
{
	forward(transform, -backward);
}

template<typename T>
Vector<T,3> up(const Matrix<T,4,4>& transform) noexcept
{
	Vector<T,3> temp;
	temp[0] = transform.at(0, 1);
	temp[1] = transform.at(1, 1);
	temp[2] = transform.at(2, 1);
	return temp;
}

template<typename T>
void up(Matrix<T,4,4>& transform, const Vector<T,3>& up) noexcept
{
	transform.set(0, 1, up[0]);
	transform.set(1, 1, up[1]);
	transform.set(2, 1, up[2]);
}

template<typename T>
Vector<T,3> down(const Matrix<T,4,4>& transform) noexcept
{
	return -up(transform);
}

template<typename T>
void down(Matrix<T,4,4>& transform, const Vector<T,3>& down) noexcept
{
	up(transform, -down);
}

template<typename T>
Vector<T,3> right(const Matrix<T,4,4>& transform) noexcept
{
	Vector<T,3> temp;
	temp[0] = transform.at(0, 0);
	temp[1] = transform.at(1, 0);
	temp[2] = transform.at(2, 0);
	return temp;
}

template<typename T>
void right(Matrix<T,4,4>& transform, const Vector<T,3>& right) noexcept
{
	transform.set(0, 0, right[0]);
	transform.set(1, 0, right[1]);
	transform.set(2, 0, right[2]);
}

template<typename T>
Vector<T,3> left(const Matrix<T,4,4>& transform) noexcept
{
	return -right(transform);
}

template<typename T>
void left(Matrix<T,4,4>& transform, const Vector<T,3>& left) noexcept
{
	right(transform, -left);
}

} // namespace sfz

#include "sfz/MSVC12HackOFF.hpp"