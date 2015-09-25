namespace sfz {

// AABB2D: Constructors and destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline AABB2D::AABB2D(vec2 centerPos, vec2 dimensions) noexcept
:
	min{centerPos - (dimensions/2.0f)},
	max{centerPos + (dimensions/2.0f)}
{ }

inline AABB2D::AABB2D(float centerX, float centerY, float width, float height) noexcept
:
	min{vec2{centerX - (width/2.0f), centerY - (height/2.0f)}},
	max{vec2{centerX + (width/2.0f), centerY + (height/2.0f)}}
{ }

// AABB2D: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline size_t AABB2D::hash() const noexcept
{
	std::hash<vec2> hasher;
	size_t hash = 0;
	// hash_combine algorithm from boost
	hash ^= hasher(min) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= hasher(max) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	return hash;
}

inline std::string AABB2D::to_string() const noexcept
{
	std::string str;
	str += "[min=";
	str += sfz::to_string(min);
	str += ", max=";
	str += sfz::to_string(max);
	str += "]";
	return std::move(str);
}

// AABB2D: Comparison operators
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline bool AABB2D::operator== (const AABB2D& other) const noexcept
{
	return min == other.min && max == other.max;
}

inline bool AABB2D::operator!= (const AABB2D& other) const noexcept
{
	return !((*this) == other);
}

// Free (non-member) operators
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline std::ostream& operator<< (std::ostream& ostream, const AABB2D& aabb) noexcept
{
	return ostream << aabb.to_string();
}

} // namespace sfz

// Specializations of standard library for sfz::AABB2D
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
namespace std {

inline size_t hash<sfz::AABB2D>::operator() (const sfz::AABB2D& aabb) const noexcept
{
	return aabb.hash();
}

} // namespace std