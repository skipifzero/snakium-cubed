#include "gamelogic/Direction.hpp"

#include <sfz/Assert.hpp>
#include <sfz/math/Matrix.hpp>

namespace s3 {

using sfz::mat3i;

// Direction functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Direction opposite(Direction direction) noexcept
{
	switch (direction) {
	case Direction::BACKWARD: return Direction::FORWARD;
	case Direction::FORWARD: return Direction::BACKWARD;
	case Direction::UP: return Direction::DOWN;
	case Direction::DOWN: return Direction::UP;
	case Direction::RIGHT: return Direction::LEFT;
	case Direction::LEFT: return Direction::RIGHT;
	}
	sfz_error("Impossible enum");
}


vec3i toIntVector(Direction direction) noexcept
{
	switch (direction) {
	case Direction::BACKWARD: return vec3i{0, 0, 1};
	case Direction::FORWARD: return vec3i{0, 0, -1};
	case Direction::UP: return vec3i{0, 1, 0};
	case Direction::DOWN: return vec3i{0, -1, 0};
	case Direction::RIGHT: return vec3i{1, 0, 0};
	case Direction::LEFT: return vec3i{-1, 0, 0};
	}
	sfz_error("Impossible enum");
}

Direction fromIntVector(const vec3i& vector) noexcept
{
	if (vector == vec3i{0, 0, 1}) return Direction::BACKWARD;
	if (vector == vec3i{0, 0, -1}) return Direction::FORWARD;
	if (vector == vec3i{0, 1, 0}) return Direction::UP;
	if (vector == vec3i{0, -1, 0}) return Direction::DOWN;
	if (vector == vec3i{1, 0, 0}) return Direction::RIGHT;
	if (vector == vec3i{-1, 0, 0}) return Direction::LEFT;
	sfz_error("Invalid int vector");
}

vec3 toVector(Direction direction) noexcept
{
	switch (direction) {
	case Direction::BACKWARD: return vec3{0.0f, 0.0f, 1.0f};
	case Direction::FORWARD: return vec3{0.0f, 0.0f, -1.0f};
	case Direction::UP: return vec3{0.0f, 1.0f, 0.0f};
	case Direction::DOWN: return vec3{0.0f, -1.0f, 0.0f};
	case Direction::RIGHT: return vec3{1.0f, 0.0f, 0.0f};
	case Direction::LEFT: return vec3{-1.0f, 0.0f, 0.0f};
	}
	sfz_error("Impossible enum");
}

const char* to_string(Direction direction) noexcept
{
	switch (direction) {
	case Direction::BACKWARD: return "BACKWARD";
	case Direction::FORWARD: return "FORWARD";
	case Direction::UP: return "UP";
	case Direction::DOWN: return "DOWN";
	case Direction::RIGHT: return "RIGHT";
	case Direction::LEFT: return "LEFT";
	}
	sfz_error("Impossible enum");
}

Direction left(Direction side, Direction up) noexcept
{
	sfz_assert_debug(side != up && side != opposite(side));
	switch (side) {
	case Direction::BACKWARD:
		switch (up) {
		case Direction::UP: return Direction::LEFT;
		case Direction::DOWN: return Direction::RIGHT;
		case Direction::RIGHT: return Direction::UP;
		case Direction::LEFT: return Direction::DOWN;
		default: break;
		}
	case Direction::FORWARD:
		switch (up) {
		case Direction::UP: return Direction::RIGHT;
		case Direction::DOWN: return Direction::LEFT;
		case Direction::RIGHT: return Direction::DOWN;
		case Direction::LEFT: return Direction::UP;
		default: break;
		}
	case Direction::UP:
		switch (up) {
		case Direction::BACKWARD: return Direction::RIGHT;
		case Direction::FORWARD: return Direction::LEFT;
		case Direction::RIGHT: return Direction::FORWARD;
		case Direction::LEFT: return Direction::BACKWARD;
		default: break;
		}
	case Direction::DOWN:
		switch (up) {
		case Direction::BACKWARD: return Direction::LEFT;
		case Direction::FORWARD: return Direction::RIGHT;
		case Direction::RIGHT: return Direction::BACKWARD;
		case Direction::LEFT: return Direction::FORWARD;
		default: break;
		}
	case Direction::RIGHT:
		switch (up) {
		case Direction::BACKWARD: return Direction::DOWN;
		case Direction::FORWARD: return Direction::UP;
		case Direction::UP: return Direction::BACKWARD;
		case Direction::DOWN: return Direction::FORWARD;
		default: break;
		}
	case Direction::LEFT:
		switch (up) {
		case Direction::BACKWARD: return Direction::UP;
		case Direction::FORWARD: return Direction::DOWN;
		case Direction::UP: return Direction::FORWARD;
		case Direction::DOWN: return Direction::BACKWARD;
		default: break;
		}
	}
	sfz_error("Invalid input");
}

Direction right(Direction side, Direction up) noexcept
{
	return opposite(left(side, up));
}

bool isLeftTurn(Direction side, Direction from, Direction to) noexcept
{
	Direction leftDir = left(side, opposite(from));
	return leftDir == to;
}

bool isRightTurn(Direction side, Direction from, Direction to) noexcept
{
	Direction rightDir = right(side, opposite(from));
	return rightDir == to;
}

Direction defaultUp(Direction side) noexcept
{
	switch (side) {
	case Direction::BACKWARD: return Direction::UP;
	case Direction::FORWARD: return Direction::UP;
	case Direction::UP: return Direction::BACKWARD;
	case Direction::DOWN: return Direction::BACKWARD;
	case Direction::RIGHT: return Direction::UP;
	case Direction::LEFT: return Direction::UP;
	}
	sfz_error("Invalid enum");
}


Direction remap(Direction fromSide, Direction fromUp, Direction toSide, Direction toUp, Direction fromDir) noexcept
{
	sfz_assert_debug(fromSide != fromUp && fromSide != opposite(fromUp));
	sfz_assert_debug(toSide != toUp && toSide != opposite(toUp));

	mat3i unitToTo;
	unitToTo.setColumn(0, toIntVector(right(toSide, toUp)));
	unitToTo.setColumn(1, toIntVector(toUp));
	unitToTo.setColumn(2, toIntVector(toSide));

	mat3i unitToFrom;
	unitToFrom.setColumn(0, toIntVector(right(fromSide, fromUp)));
	unitToFrom.setColumn(1, toIntVector(fromUp));
	unitToFrom.setColumn(2, toIntVector(fromSide));

	return fromIntVector(unitToTo * transpose(unitToFrom) * toIntVector(fromDir));
}

Direction remapDefaultUp(Direction fromSide, Direction toSide, Direction fromDir) noexcept
{
	return remap(fromSide, defaultUp(fromSide), toSide, defaultUp(toSide), fromDir);
}


// 3D Direction enum
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/*Direction3D up(Direction3D side, Direction3D sideRelativeUp) noexcept
{
	if (side == sideRelativeUp || side == opposite(sideRelativeUp)) {
		std::cerr << "Invalid side relative up direction, side == " << side
		          << ", sideRelativeUp == " << sideRelativeUp << std::endl;
		std::terminate();
	}
	return sideRelativeUp;
}

Direction3D down(Direction3D side, Direction3D sideRelativeUp) noexcept
{
	if (side == sideRelativeUp || side == opposite(sideRelativeUp)) {
		std::cerr << "Invalid side relative up direction, side == " << side
		          << ", sideRelativeUp == " << sideRelativeUp << std::endl;
		std::terminate();
	}
	return opposite(sideRelativeUp);
}

Direction3D left(Direction3D side, Direction3D sideRelativeUp) noexcept
{
	if (side == sideRelativeUp || side == opposite(sideRelativeUp)) {
		std::cerr << "Invalid side relative up direction, side == " << side
		          << ", sideRelativeUp == " << sideRelativeUp << std::endl;
		std::terminate();
	}
	switch (side) {
	case Direction3D::NORTH:
		switch (sideRelativeUp) {
		case Direction3D::WEST: return Direction3D::UP;
		case Direction3D::EAST: return Direction3D::DOWN;
		case Direction3D::UP: return Direction3D::EAST;
		case Direction3D::DOWN: return Direction3D::WEST;
		default: break;
		}
	case Direction3D::SOUTH:
		switch (sideRelativeUp) {
		case Direction3D::WEST: return Direction3D::DOWN;
		case Direction3D::EAST: return Direction3D::UP;
		case Direction3D::UP: return Direction3D::WEST;
		case Direction3D::DOWN: return Direction3D::EAST;
		default: break;
		}
	case Direction3D::WEST:
		switch (sideRelativeUp) {
		case Direction3D::NORTH: return Direction3D::DOWN;
		case Direction3D::SOUTH: return Direction3D::UP;
		case Direction3D::UP: return Direction3D::NORTH;
		case Direction3D::DOWN: return Direction3D::SOUTH;
		default: break;
		}
	case Direction3D::EAST:
		switch (sideRelativeUp) {
		case Direction3D::NORTH: return Direction3D::UP;
		case Direction3D::SOUTH: return Direction3D::DOWN;
		case Direction3D::UP: return Direction3D::SOUTH;
		case Direction3D::DOWN: return Direction3D::NORTH;
		default: break;
		}
	case Direction3D::UP:
		switch (sideRelativeUp) {
		case Direction3D::NORTH: return Direction3D::WEST;
		case Direction3D::SOUTH: return Direction3D::EAST;
		case Direction3D::WEST: return Direction3D::SOUTH;
		case Direction3D::EAST: return Direction3D::NORTH;
		default: break;
		}
	case Direction3D::DOWN:
		switch (sideRelativeUp) {
		case Direction3D::NORTH: return Direction3D::EAST;
		case Direction3D::SOUTH: return Direction3D::WEST;
		case Direction3D::WEST: return Direction3D::NORTH;
		case Direction3D::EAST: return Direction3D::SOUTH;
		default: break;
		}
	}
	std::cerr << "This should never happen." << std::endl;
	std::terminate();
}

Direction3D right(Direction3D side, Direction3D sideRelativeUp) noexcept
{
	return opposite(left(side, sideRelativeUp));
}

Direction3D defaultUp(Direction3D side) noexcept
{
	switch (side) {
	case Direction3D::NORTH: return Direction3D::UP;
	case Direction3D::SOUTH: return Direction3D::UP;
	case Direction3D::WEST: return Direction3D::UP;
	case Direction3D::EAST: return Direction3D::UP;
	case Direction3D::UP: return Direction3D::SOUTH;
	case Direction3D::DOWN: return Direction3D::SOUTH;
	}
}

Direction3D map(Direction3D side, Direction3D sideRelativeUp, Direction2D dir) noexcept
{
	switch (dir) {
	case Direction2D::UP: return up(side, sideRelativeUp);
	case Direction2D::DOWN: return down(side, sideRelativeUp);
	case Direction2D::LEFT: return left(side, sideRelativeUp);
	case Direction2D::RIGHT: return right(side, sideRelativeUp);
	}
}

Direction3D mapDefaultUp(Direction3D side, Direction2D dir) noexcept
{
	return map(side, defaultUp(side), dir);
}

Direction2D unMap(Direction3D side, Direction3D sideRelativeUp, Direction3D dir) noexcept
{
	if (side == sideRelativeUp || side == opposite(sideRelativeUp)) {
		std::cerr << "Invalid side relative up direction, side == " << side
		          << ", sideRelativeUp == " << sideRelativeUp << std::endl;
		std::terminate();
	}
	
	if (dir == sideRelativeUp) return Direction2D::UP;
	if (dir == opposite(sideRelativeUp)) return Direction2D::DOWN;
	Direction3D leftDir = left(side, sideRelativeUp);
	if (dir == leftDir) return Direction2D::LEFT;
	if (dir == opposite(leftDir)) return Direction2D::RIGHT;

	std::cerr << "This should never happen." << std::endl;
	std::terminate();
}

Direction2D unMapDefaultUp(Direction3D side, Direction3D dir) noexcept
{
	return unMap(side, defaultUp(side), dir);
}

vec3 toVector(Direction3D direction) noexcept
{
	switch (direction) {
	case Direction3D::NORTH: return sfz::vec3{0.0f, 0.0f, -1.0f};
	case Direction3D::SOUTH: return sfz::vec3{0.0f, 0.0f, 1.0f};
	case Direction3D::WEST: return sfz::vec3{-1.0f, 0.0f, 0.0f};
	case Direction3D::EAST: return sfz::vec3{1.0f, 0.0f, 0.0f};
	case Direction3D::UP: return sfz::vec3{0.0f, 1.0f, 0.0f};
	case Direction3D::DOWN: return sfz::vec3{0.0f, -1.0f, 0.0f};
	}
}

const char* to_string(const Direction3D& direction) noexcept
{
	switch (direction) {
	case Direction3D::NORTH: return "NORTH";
	case Direction3D::SOUTH: return "SOUTH";
	case Direction3D::WEST: return "WEST";
	case Direction3D::EAST: return "EAST";
	case Direction3D::UP: return "UP";
	case Direction3D::DOWN: return "DOWN";
	}
}

std::ostream& operator<< (std::ostream& ostream, const Direction3D& direction) noexcept
{
	return ostream << to_string(direction);
}*/

} // namespace s3