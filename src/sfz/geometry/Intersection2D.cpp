#include "sfz/geometry/Intersection2D.hpp"

namespace sfz {

bool pointInside(const Circle& circle, vec2 point) noexcept
{
	// If the length from the circles center to the specified point is shorter than or equal to
	// the radius then the Circle overlaps the point. Both sides of the equation is squared to
	// avoid somewhat expensive sqrt() function.
	return squaredLength(point - circle.pos) <= (circle.radius*circle.radius);
}

bool pointInside(const Rectangle& rect, vec2 point) noexcept
{
	float rectXLeft = rect.pos.x - std::abs(rect.dim.x/2.0f);
	float rectXRight = rectXLeft + std::abs(rect.dim.x);
	float rectYBottom = rect.pos.y - std::abs(rect.dim.y/2.0f);
	float rectYTop = rectYBottom + std::abs(rect.dim.y);

	return rectXLeft <= point.x && rectXRight >= point.x &&
	       rectYBottom <= point.y && rectYTop >= point.y;
}

bool overlaps(const Circle& lhs, const Circle& rhs) noexcept
{
	// If the length between the center of the two circles is less than or equal to the the sum of
	// the circle's radiuses they overlap. Both sides of the equation is squared to avoid somewhat 
	// expensive sqrt() function.
	float distSquared = squaredLength(lhs.pos - rhs.pos);
	float radiusSum = std::abs(lhs.radius) + std::abs(rhs.radius);
	return distSquared <= (radiusSum * radiusSum);
}

bool overlaps(const Rectangle& lhs, const Rectangle& rhs) noexcept
{
	float lhsXLeft = lhs.pos.x - std::abs(lhs.dim.x/2.0f);
	float lhsXRight = lhsXLeft + std::abs(lhs.dim.x);
	float lhsYBottom = lhs.pos.y - std::abs(lhs.dim.y/2.0f);
	float lhsYTop = lhsYBottom + std::abs(lhs.dim.y);

	float rhsXLeft = rhs.pos.x - std::abs(rhs.dim.x/2.0f);
	float rhsXRight = rhsXLeft + std::abs(rhs.dim.x);
	float rhsYBottom = rhs.pos.y - std::abs(rhs.dim.y/2.0f);
	float rhsYTop = rhsYBottom + std::abs(rhs.dim.y);

	return lhsXLeft   <= rhsXRight &&
	       lhsXRight  >= rhsXLeft &&
	       lhsYBottom <= rhsYTop &&
	       lhsYTop    >= rhsYBottom;
}

bool overlaps(const Circle& circle, const Rectangle& rect) noexcept
{
	float rectXLeft = rect.pos.x - std::abs(rect.dim.x/2.0f);
	float rectXRight = rectXLeft + std::abs(rect.dim.x);
	float rectYBottom = rect.pos.y - std::abs(rect.dim.y/2.0f);
	float rectYTop = rectYBottom + std::abs(rect.dim.y);

	// If the length between the center of the circle and the closest point on the rectangle is
	// less than or equal to the circles radius they overlap. Both sides of the equation is 
	// squared to avoid somewhat expensive sqrt() function. 
	float closestX = circle.pos.x;
	float closestY = circle.pos.y;
	
	// TODO: These can be optimized with min/max.
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

bool overlaps(const Rectangle& rect, const Circle& circle) noexcept
{
	return overlaps(rect, circle);
}

} // namespace sfz