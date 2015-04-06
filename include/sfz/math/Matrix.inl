#include "sfz/MSVC12HackON.hpp"

namespace sfz {

// Public constants
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T, size_t M, size_t N>
const Matrix<T,M,N>& Matrix<T,M,N>::ZERO() noexcept
{
	static const Matrix<T,M,N> zero = []() -> Matrix<T,M,N> {
		Matrix<T,M,N> tmp;
		tmp.fill(T(0));
		return tmp;
	}();
	return zero; 
}

// Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T, size_t M, size_t N>
Matrix<T,M,N>::Matrix(std::initializer_list<std::initializer_list<T>> list) noexcept
{
	sfz_assert_debug(list.size() <= M);
	size_t i = 0;
	for (auto& rowList : list) {
		sfz_assert_debug(rowList.size() <= N);
		size_t j = 0;
		for (T element : rowList) {
			mElements[j][i] = element;
			j++;
		}
		while (j < N) {
			mElements[j][i] = 0;
			j++;
		}
		i++;
	}
	while (i < M) {
		for (size_t j = 0; j < N; j++) {
			mElements[j][i] = 0;
		}
		i++;
	}
}

// Member functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T, size_t M, size_t N>
T& Matrix<T,M,N>::at(size_t i, size_t j) noexcept
{
	sfz_assert_debug(i < M);
	sfz_assert_debug(j < N);
	return mElements[j][i];
}

template<typename T, size_t M, size_t N>
T Matrix<T,M,N>::at(size_t i, size_t j) const noexcept
{
	sfz_assert_debug(i < M);
	sfz_assert_debug(j < N);
	return mElements[j][i];
}

template<typename T, size_t M, size_t N>
Vector<T,N> Matrix<T,M,N>::rowAt(size_t i) const noexcept
{
	sfz_assert_debug(i < M);
	Vector<T,N> row;
	for (size_t j = 0; j < N; j++) {
		row[j] = mElements[j][i];
	}
	return row;
}

template<typename T, size_t M, size_t N>
Vector<T,M> Matrix<T,M,N>::columnAt(size_t j) const noexcept
{
	sfz_assert_debug(j < N);
	Vector<T,M> column;
	for (size_t i = 0; i < N; i++) {
		column[i] = mElements[j][i];
	}
	return column;
}

template<typename T, size_t M, size_t N>
void Matrix<T,M,N>::set(size_t i, size_t j, T value) noexcept
{
	sfz_assert_debug(i < M);
	sfz_assert_debug(j < N);
	mElements[j][i] = value;
}

template<typename T, size_t M, size_t N>
void Matrix<T,M,N>::setRow(size_t i, const Vector<T,N>& row) noexcept
{
	sfz_assert_debug(i < M);
	size_t j = 0;
	for (auto element : row) {
		mElements[j][i] = element;
		j++;
	}
}

template<typename T, size_t M, size_t N>
void Matrix<T,M,N>::setColumn(size_t j, const Vector<T,M>& column) noexcept
{
	sfz_assert_debug(j < N);
	size_t i = 0;
	for (auto element : column) {
		mElements[j][i] = element;
		i++;
	}
}

template<typename T, size_t M, size_t N>
void Matrix<T,M,N>::fill(const T value) noexcept
{
	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			mElements[j][i] = value;
		}
	}
}

template<typename T, size_t M, size_t N>
Matrix<T,M,N> Matrix<T,M,N>::elemMult(const Matrix<T,M,N>& other) const noexcept
{
	Matrix<T,M,N> resMatrix;
	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			resMatrix.mElements[j][i] = mElements[j][i] * other.mElements[j][i];
		}
	}
	return resMatrix;
}

template<typename T, size_t M, size_t N>
Matrix<T,N,M> Matrix<T,M,N>::transpose() const noexcept
{
	Matrix<T,N,M> resMatrix;
	for (size_t i = 0; i < N; i++) {
		for (size_t j = 0; j < M; j++) {
			resMatrix.mElements[j][i] = mElements[i][j];
		}
	}
	return resMatrix;
}

template<typename T, size_t M, size_t N>
size_t Matrix<T,M,N>::hash() const noexcept
{
	std::hash<T> hasher;
	size_t hash = 0;
	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			// hash_combine algorithm from boost
			hash ^= hasher(mElements[j][i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}
	}
	return hash;
}

template<typename T, size_t M, size_t N>
std::string Matrix<T,M,N>::to_string() const noexcept
{
	using std::to_string;
	std::string str;
	str += "{ ";
	for (size_t i = 0; i < M; i++) {
		if (i > 0) {
			str += "  ";
		}
		str += "{";
		for (size_t j = 0; j < N; j++) {
			str += to_string(at(i, j));
			if (j < N-1) {
				str += ", ";
			}
		}
		str += "}";
		if (i < M-1) {
			str += ",\n";
		}
	}
	str += " }";
	return std::move(str);
}

// Operators (arithmetic & sssignment)
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T, size_t M, size_t N>
Matrix<T,M,N>& Matrix<T,M,N>::operator+= (const Matrix<T,M,N>& other) noexcept
{
	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			mElements[j][i] += other.mElements[j][i];
		}
	}
	return *this;
}

template<typename T, size_t M, size_t N>
Matrix<T,M,N>& Matrix<T,M,N>::operator-= (const Matrix<T,M,N>& other) noexcept
{
	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			mElements[j][i] -= other.mElements[j][i];
		}
	}
	return *this;
}

template<typename T, size_t M, size_t N>
Matrix<T,M,N>& Matrix<T,M,N>::operator*= (const T& other) noexcept
{
	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			mElements[j][i] *= other;
		}
	}
	return *this;
}

// Operators (arithmetic)
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T, size_t M, size_t N>
Matrix<T,M,N> Matrix<T,M,N>::operator+ (const Matrix<T,M,N>& other) const noexcept
{
	return Matrix<T,M,N>{*this} += other;
}

template<typename T, size_t M, size_t N>
Matrix<T,M,N> Matrix<T,M,N>::operator- (const Matrix<T,M,N>& other) const noexcept
{
	return Matrix<T,M,N>{*this} -= other;
}

template<typename T, size_t M, size_t N>
Matrix<T,M,N> Matrix<T,M,N>::operator- () const noexcept
{
	return Matrix<T,M,N>{*this} *= -1;
}

template<typename T, size_t M, size_t N>
template<size_t P>
Matrix<T,M,P> Matrix<T,M,N>::operator* (const Matrix<T,N,P>& other) const noexcept
{
	Matrix<T,M,P> resMatrix;
	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < P; j++) {
			T temp = 0;
			size_t jInnerThis = 0;
			size_t iInnerOther = 0;
			while (jInnerThis < M) {
				temp += mElements[jInnerThis][i] * other.mElements[j][iInnerOther];
				jInnerThis++;
				iInnerOther++;
			}
			resMatrix.mElements[j][i] = temp;
		}
	}
	return resMatrix;
}

template<typename T, size_t M, size_t N>
Vector<T,M> Matrix<T,M,N>::operator* (const Vector<T,N>& vector) const noexcept
{
	Vector<T,M> resVector;
	for (size_t i = 0; i < M; i++) {
		T temp = 0;
		size_t jInnerThis = 0;
		for (T vecElem : vector) {
			temp += mElements[jInnerThis][i] * vecElem;
			jInnerThis++;
		}
		resVector[i] = temp;
	}
	return resVector;
}

template<typename T, size_t M, size_t N>
Matrix<T,M,N> Matrix<T,M,N>::operator* (const T& other) const noexcept
{
	return Matrix<T,M,N>{*this} *= other;
}

// Operators (comparison)
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T, size_t M, size_t N>
bool Matrix<T,M,N>::operator== (const Matrix<T,M,N>& other) const noexcept
{
	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			if (mElements[j][i] != other.mElements[j][i]) {
				return false;
			}
		}
	}
	return true;
}

template<typename T, size_t M, size_t N>
bool Matrix<T,M,N>::operator!= (const Matrix<T,M,N>& other) const noexcept
{
	return !((*this) == other);
}

// Non-member operators (arithmetic)
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T, size_t N>
Matrix<T,N,N>& operator*= (Matrix<T,N,N>& lhs, const Matrix<T,N,N>& rhs) noexcept
{
	return (lhs = lhs * rhs);
}

template<typename T, size_t M, size_t N>
Matrix<T,M,N> operator* (const T& lhs, const Matrix<T,M,N>& rhs) noexcept
{
	return rhs * lhs;
}

// Non-member operators (others)
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

template<typename T, size_t M, size_t N>
std::ostream& operator<< (std::ostream& ostream, const Matrix<T,M,N>& matrix) noexcept
{
	return ostream << matrix.to_string();
}

} // namespace sfz

// Specializations of standard library for sfz::Matrix
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

namespace std {

template<typename T, size_t M, size_t N>
size_t hash<sfz::Matrix<T,M,N>>::operator() (const sfz::Matrix<T,M,N>& matrix) const noexcept
{
	return matrix.hash();
}

} // namespace std

#include "sfz/MSVC12HackOFF.hpp"