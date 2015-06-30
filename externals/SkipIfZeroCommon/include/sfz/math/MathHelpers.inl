namespace sfz {

template<typename T>
bool approxEqual(T lhs, T rhs, T epsilon) noexcept
{
	static_assert(std::is_floating_point<T>::value, "Must be floating point type.");
	return lhs <= rhs + epsilon && lhs >= rhs - epsilon;
}

template<typename T>
bool approxEqual(T lhs, T rhs) noexcept
{
	return approxEqual<T>(lhs, rhs, defaultEpsilon<T>());
}

template<typename T, size_t N>
bool approxEqual(const Vector<T,N>& lhs, const Vector<T,N>& rhs, T epsilon) noexcept
{
	for (size_t i = 0; i < N; i++) {
		if(!approxEqual<T>(lhs[i], rhs[i], epsilon)) return false;
	}
	return true;
}

template<typename T, size_t N>
bool approxEqual(const Vector<T,N>& lhs, const Vector<T,N>& rhs) noexcept
{
	return approxEqual<T,N>(lhs, rhs, defaultEpsilon<T>());
}

template<typename T, size_t M, size_t N>
bool approxEqual(const Matrix<T,M,N>& lhs, const Matrix<T,M,N>& rhs, T epsilon) noexcept
{
	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			if (!approxEqual<T>(lhs.at(i,j), rhs.at(i,j), epsilon)) return false;
		}
	}
	return true;
}

template<typename T, size_t M, size_t N>
bool approxEqual(const Matrix<T,M,N>& lhs, const Matrix<T,M,N>& rhs) noexcept
{
	return approxEqual<T,M,N>(lhs, rhs, defaultEpsilon<T>());
}

template<typename ArgT, typename FloatT>
ArgT lerp(const ArgT& v0, const ArgT& v1, FloatT t) noexcept
{
	return (FloatT(1)-t)*v0 + t*v1;
}

} // namespace sfz
