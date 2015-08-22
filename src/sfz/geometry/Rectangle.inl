namespace sfz {

// Constructors and destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline Rectangle::Rectangle(vec2 position, vec2 dimensions) noexcept
:
	pos{position},
	dim{dimensions}
{ }

inline Rectangle::Rectangle(float x, float y, float width, float height) noexcept
:
	pos{x, y},
	dim{width, height}
{ }

// Public member functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline size_t Rectangle::hash() const noexcept
{
	std::hash<float> hasher;
	size_t hash = 0;
	// hash_combine algorithm from boost
	hash ^= hasher(pos.x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= hasher(pos.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= hasher(dim.x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= hasher(dim.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	return hash;
}

inline std::string Rectangle::to_string() const noexcept
{
	std::string str;
	str += "[pos=";
	str += sfz::to_string(pos);
	str += ", dim=";
	str += sfz::to_string(dim);
	str += "]";
	return std::move(str);
}

// Comparison operators
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline bool Rectangle::operator== (const Rectangle& other) const noexcept
{
	return pos == other.pos && dim == other.dim;
}

inline bool Rectangle::operator!= (const Rectangle& other) const noexcept
{
	return !((*this) == other);
}

// Free (non-member) operators
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline std::ostream& operator<< (std::ostream& ostream, const Rectangle rect) noexcept
{
	return ostream << rect.to_string();
}

} // namespace sfz

// Specializations of standard library for sfz::Rectangle
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
namespace std {

inline size_t hash<sfz::Rectangle>::operator() (const sfz::Rectangle& rect) const noexcept
{
	return rect.hash();
}

} // namespace std