namespace sfz {

// Constructors and destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline Circle::Circle(vec2 pos, float radius) noexcept
:
	pos{pos},
	radius{radius}
{ }

inline Circle::Circle(float x, float y, float radius) noexcept
:
	pos{x, y},
	radius{radius}
{ }

// Public member functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline size_t Circle::hash() const noexcept
{
	std::hash<float> hasher;
	size_t hash = 0;
	// hash_combine algorithm from boost
	hash ^= hasher(pos[0]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= hasher(pos[1]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= hasher(radius) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	return hash;
}

inline std::string Circle::to_string() const noexcept
{
	std::string str;
	str += "[pos=";
	str += sfz::to_string(pos);
	str += ", r=";
	str += std::to_string(radius);
	str += "]";
	return std::move(str);
}


// Comparison operators
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline bool Circle::operator== (const Circle& other) const noexcept
{
	return pos == other.pos && radius == other.radius;
}

inline bool Circle::operator!= (const Circle& other) const noexcept
{
	return !((*this) == other);
}

// Free (non-member) operators
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

inline std::ostream& operator<< (std::ostream& ostream, const Circle circle) noexcept
{
	return ostream << circle.to_string();
}

} // namespace sfz

// Specializations of standard library for sfz::Circle
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
namespace std {

inline size_t hash<sfz::Circle>::operator() (const sfz::Circle& circle) const noexcept
{
	return circle.hash();
}

} // namespace std