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

DirectionInput opposite(DirectionInput direction) noexcept
{
	switch (direction) {
	case DirectionInput::UP: return DirectionInput::DOWN;
	case DirectionInput::DOWN: return DirectionInput::UP;
	case DirectionInput::LEFT: return DirectionInput::RIGHT;
	case DirectionInput::RIGHT: return DirectionInput::LEFT;
	case DirectionInput::DIVE: return DirectionInput::DIVE;
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
	if (side == from || side == opposite(from)) return false;
	Direction leftDir = left(side, opposite(from));
	return leftDir == to;
}

bool isRightTurn(Direction side, Direction from, Direction to) noexcept
{
	if (side == from || side == opposite(from)) return false;
	Direction rightDir = right(side, opposite(from));
	return rightDir == to;
}

bool isDive(Direction side, Direction to) noexcept
{
	return to == opposite(side);
}

bool isAscend(Direction side, Direction from) noexcept
{
	return from == opposite(side);
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

} // namespace s3