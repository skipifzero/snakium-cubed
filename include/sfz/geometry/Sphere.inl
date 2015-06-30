namespace sfz {

// Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline Sphere::Sphere(const vec3& center, float radius) noexcept
:
	mCenter(center),
	mRadius{radius}
{
	sfz_assert_debug(radius > 0.0f);
}

// Public member functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline vec3 Sphere::closestPoint(const vec3& point) const noexcept
{
	const vec3 distToPoint = point - mCenter;
	vec3 res = point;
	if (squaredLength(distToPoint) > mRadius*mRadius)
	{
		res = mCenter + normalize(distToPoint)*mRadius;
	}
	return res;
}

inline size_t Sphere::hash() const noexcept
{
	std::hash<vec3> vecHasher;
	std::hash<float> floatHasher;
	size_t hash = 0;
	// hash_combine algorithm from boost
	hash ^= vecHasher(mCenter) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= floatHasher(mRadius) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	return hash;
}

inline std::string Sphere::to_string() const noexcept
{
	std::string str{"Center: "};
	str += sfz::to_string(mCenter);
	str += "\nRadius: ";
	str += std::to_string(mRadius);
	return std::move(str);
}

// Public getters/setters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline void Sphere::radius(float newRadius) noexcept
{
	sfz_assert_debug(newRadius > 0.0f);
	mRadius = newRadius;
}

// Non-member operators
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline std::ostream& operator<< (std::ostream& ostream, const Sphere& sphere) noexcept
{
	return ostream << sphere.to_string();
}

} // namespace sfz

// Specializations of standard library for sfz::Sphere
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

namespace std {

inline size_t hash<sfz::Sphere>::operator() (const sfz::Sphere& sphere) const noexcept
{
	return sphere.hash();
}

} // namespace std
