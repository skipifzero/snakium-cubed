#include "gamelogic/Direction.hpp"

#include <exception> // std::terminate
#include <iostream>

namespace s3 {

// 2D Direction enum
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Direction2D opposite(Direction2D direction) noexcept
{
	switch (direction) {
	case Direction2D::UP: return Direction2D::DOWN;
	case Direction2D::DOWN: return Direction2D::UP;
	case Direction2D::LEFT: return Direction2D::RIGHT;
	case Direction2D::RIGHT: return Direction2D::LEFT;
	}
}

Direction2D toLeft(Direction2D from) noexcept
{
	switch (from) {
	case Direction2D::UP: return Direction2D::RIGHT;
	case Direction2D::DOWN: return Direction2D::LEFT;
	case Direction2D::LEFT: return Direction2D::UP;
	case Direction2D::RIGHT: return Direction2D::DOWN;
	}
}

Direction2D toRight(Direction2D from) noexcept
{
	switch (from) {
	case Direction2D::UP: return Direction2D::LEFT;
	case Direction2D::DOWN: return Direction2D::RIGHT;
	case Direction2D::LEFT: return Direction2D::DOWN;
	case Direction2D::RIGHT: return Direction2D::UP;
	}
}

bool isTurn(Direction2D from, Direction2D to) noexcept
{
	return from != to && from != opposite(to);
}

bool isLeftTurn(Direction2D from, Direction2D to) noexcept
{
	return toLeft(from) == to;
}

bool isRightTurn(Direction2D from, Direction2D to) noexcept
{
	return toRight(from) == to;
}

const char* to_string(const Direction2D& direction) noexcept
{
	switch (direction) {
	case Direction2D::UP: return "UP";
	case Direction2D::DOWN: return "DOWN";
	case Direction2D::LEFT: return "LEFT";
	case Direction2D::RIGHT: return "RIGHT";
	}
}

std::ostream& operator<< (std::ostream& ostream, const Direction2D& direction) noexcept
{
	return ostream << to_string(direction);
}

// 3D Direction enum
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Direction3D opposite(Direction3D direction) noexcept
{
	uint8_t dir = static_cast<uint8_t>(direction);
	if (dir % 2 == 0) {
		return static_cast<Direction3D>(dir + 1);
	} else {
		return static_cast<Direction3D>(dir - 1);
	}
	/*switch (direction) {
	case Direction3D::NORTH: return Direction3D::SOUTH;
	case Direction3D::SOUTH: return Direction3D::NORTH;
	case Direction3D::WEST: return Direction3D::EAST;
	case Direction3D::EAST: return Direction3D::WEST;
	case Direction3D::UP: return Direction3D::DOWN;
	case Direction3D::DOWN: return Direction3D::UP;
	}*/
}

Direction3D up(Direction3D side, Direction3D sideRelativeUp) noexcept
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
}

} // namespace s3