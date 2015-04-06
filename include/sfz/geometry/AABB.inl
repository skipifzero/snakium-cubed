#include "sfz/MSVC12HackON.hpp"

namespace sfz {

// Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline AABB::AABB(const vec3f& min, const vec3f& max) noexcept
:
	mMin{min},
	mMax{max}
{
	sfz_assert_debug(min[0] < max[0]);
	sfz_assert_debug(min[1] < max[1]);
	sfz_assert_debug(min[2] < max[2]);
}

inline AABB::AABB(const vec3f& centerPos, float xExtent, float yExtent, float zExtent) noexcept
{
	sfz_assert_debug(xExtent > 0);
	sfz_assert_debug(yExtent > 0);
	sfz_assert_debug(zExtent > 0);

	vec3f temp = centerPos;
	temp[0] -= xExtent/2.0f;
	temp[1] -= yExtent/2.0f;
	temp[2] -= zExtent/2.0f;
	mMin = temp;

	temp[0] += xExtent;
	temp[1] += yExtent;
	temp[2] += zExtent;
	mMax = temp;
}

// Public member functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline std::array<vec3f,8> AABB::corners() const noexcept
{
	std::array<vec3f,8> result;
	this->corners(&result[0]);
	return result;
}

inline void AABB::corners(vec3f* arrayOut) const noexcept
{
	const vec3f xExtent = vec3f{mMax[0] - mMin[0], 0.0f, 0.0f};
	const vec3f yExtent = vec3f{0.0f, mMax[1] - mMin[1], 0.0f};
	const vec3f zExtent = vec3f{0.0f, 0.0f, mMax[2] - mMin[2]};

	arrayOut[0] = mMin; // Back-bottom-left
	arrayOut[1] = mMin + zExtent; // Front-bottom-left
	arrayOut[2] = mMin + yExtent; // Back-top-left
	arrayOut[3] = mMin + zExtent + yExtent; // Front-top-left
	arrayOut[4] = mMin + xExtent; // Back-bottom-right
	arrayOut[5] = mMin + zExtent + xExtent; // Front-bottom-right
	arrayOut[6] = mMin + yExtent + xExtent; // Back-top-right
	arrayOut[7] = mMax; // Front-top-right
}

inline vec3f AABB::closestPoint(const vec3f& point) const noexcept
{
	vec3f res = point;
	float val;
	for (size_t i = 0; i < 3; i++) {
		val = point[i];
		if (val < mMin[i]) val = mMin[i];
		if (val > mMax[i]) val = mMax[i];
		res[i] = val;
	}
	return res;
}

inline size_t AABB::hash() const noexcept
{
	std::hash<vec3f> hasher;
	size_t hash = 0;
	// hash_combine algorithm from boost
	hash ^= hasher(mMin) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= hasher(mMax) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	return hash;
}

inline std::string AABB::to_string() const noexcept
{
	std::string str{"Min: "};
	str += mMin.to_string();
	str += "\nMax: ";
	str += mMax.to_string();
	return std::move(str);
}

// Public getters/setters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline void AABB::position(const vec3f& newCenterPos) noexcept
{
	const vec3f halfExtents{xExtent()/2.0f, yExtent()/2.0f, zExtent()/2.0f};
	mMin = newCenterPos - halfExtents;
	mMax = newCenterPos + halfExtents;
}

inline void AABB::extents(const vec3f& newExtents) noexcept
{
	sfz_assert_debug(newExtents[0] > 0);
	sfz_assert_debug(newExtents[1] > 0);
	sfz_assert_debug(newExtents[2] > 0);
	const vec3f pos = position();
	const vec3f halfExtents = newExtents/2.0f;
	mMin = pos - halfExtents;
	mMin = pos + halfExtents;
}

inline void AABB::xExtent(float newXExtent) noexcept
{
	extents(vec3f{newXExtent, yExtent(), zExtent()});
}

inline void AABB::yExtent(float newYExtent) noexcept
{
	extents(vec3f{xExtent(), newYExtent, zExtent()});
}

inline void AABB::zExtent(float newZExtent) noexcept
{
	extents(vec3f{xExtent(), yExtent(), newZExtent});
}

// Non-member operators
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline std::ostream& operator<< (std::ostream& ostream, const AABB& aabb) noexcept
{
	return ostream << aabb.to_string();
}

} // namespace sfz

// Specializations of standard library for sfz::AABB
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

namespace std {

inline size_t hash<sfz::AABB>::operator() (const sfz::AABB& aabb) const noexcept
{
	return aabb.hash();
}

} // namespace std

#include "sfz/MSVC12HackOFF.hpp"