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

inline bool Rectangle::overlap(vec2 point) const noexcept
{
	float rectXLeft = pos.x - std::abs(dim.x/2.0f);
	float rectXRight = rectXLeft + std::abs(dim.x);
	float rectYBottom = pos.y - std::abs(dim.y/2.0f);
	float rectYTop = rectYBottom + std::abs(dim.y);

	return rectXLeft <= point.x && rectXRight >= point.x &&
	       rectYBottom <= point.y && rectYTop >= point.y;
}

inline bool Rectangle::overlap(const Rectangle& rect) const noexcept
{
	float thisXLeft = pos.x - std::abs(dim.x/2.0f);
	float thisXRight = thisXLeft + std::abs(dim.x);
	float thisYBottom = pos.y - std::abs(dim.y/2.0f);
	float thisYTop = thisYBottom + std::abs(dim.y);

	float otherXLeft = rect.pos.x - std::abs(rect.dim.x/2.0f);
	float otherXRight = otherXLeft + std::abs(rect.dim.x);
	float otherYBottom = rect.pos.y - std::abs(rect.dim.y/2.0f);
	float otherYTop = otherYBottom + std::abs(rect.dim.y);

	return thisXLeft   <= otherXRight &&
	       thisXRight  >= otherXLeft &&
	       thisYBottom <= otherYTop &&
	       thisYTop    >= otherYBottom;
}

inline bool Rectangle::overlap(const Circle& circle) const noexcept
{
	float rectXLeft = pos.x - std::abs(dim.x/2.0f);
	float rectXRight = rectXLeft + std::abs(dim.x);
	float rectYBottom = pos.y - std::abs(dim.y/2.0f);
	float rectYTop = rectYBottom + std::abs(dim.y);

	// If the length between the center of the circle and the closest point on the rectangle is
	// less than or equal to the circles radius they overlap. Both sides of the equation is 
	// squared to avoid somewhat expensive sqrt() function. 
	float closestX = circle.pos.x;
	float closestY = circle.pos.y;
	
	if (circle.pos.x <= rectXLeft) {
		closestX = rectXLeft;
	} 
	else if (circle.pos.x >= rectXRight) {
		closestX = rectXRight;
	}
	
	if (circle.pos.y <= rectYBottom) {
		closestY = rectYBottom;
	}
	else if (circle.pos.y >= rectYTop) {
		closestY = rectYTop;
	}
	
	return squaredLength(vec2{closestX,closestY} - circle.pos) <= (circle.radius*circle.radius);
}

inline float Rectangle::area() const noexcept
{
	return std::abs(dim.x)*std::abs(dim.y);
}

inline float Rectangle::circumference() const noexcept
{
	return std::abs(dim.x)*2 + std::abs(dim.y)*2;
}

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