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

inline bool Circle::overlap(vec2 point) const noexcept
{
	// If the length from this circles center to the specified point is shorter than or equal to
	// the radius then this Circle overlaps the point. Both sides of the equation is squared to
	// avoid somewhat expensive sqrt() function.
	return squaredNorm(point - pos) <= (radius*radius);
}

inline bool Circle::overlap(const Circle& circle) const noexcept
{
	// If the length between the center of the two circles is less than or equal to the the sum of
	// the circle's radiuses they overlap. Both sides of the equation is squared to avoid somewhat 
	// expensive sqrt() function.
	float distSquared = squaredNorm(circle.pos - pos);
	float radiusSum = std::abs(radius) + std::abs(circle.radius);
	return distSquared <= (radiusSum * radiusSum);
}

inline bool Circle::overlap(const Rectangle& rect) const noexcept
{
	return rect.overlap(*this);
}

inline float Circle::area() const noexcept
{
	return static_cast<T>(g_PI_DOUBLE)*radius*radius;
}

inline float Circle::circumference() const noexcept
{
	return static_cast<T>(2)*static_cast<T>(g_PI_DOUBLE)*std::abs(mRad);
}

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
	return pos == other.pos && radius == other.rad;
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