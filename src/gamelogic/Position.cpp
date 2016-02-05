#include "gamelogic/Position.hpp"

#include <iostream>
#include <exception> // std::terminate

namespace s3 {

bool operator== (const Position& lhs, const Position& rhs) noexcept
{
	return lhs.side == rhs.side && lhs.e1 == rhs.e1 && lhs.e2 == rhs.e2;
}

bool operator!= (const Position& lhs, const Position& rhs) noexcept
{
	return !(lhs == rhs);
}

int16_t Position::coordinate(Coordinate coord) noexcept
{
	return coord == Coordinate::e1 ? e1 : e2;
}

void Position::setCoordinate(Coordinate coord, int16_t value) noexcept
{
	if (coord == Coordinate::e1) e1 = value;
	else e2 = value;
}

template<typename T>
int sgn(T value) noexcept
{
	return (value > static_cast<T>(0)) - (value < static_cast<T>(0));
}

Direction direction(Direction side, Coordinate coordinate) noexcept
{
	switch (coordinate) {
	case Coordinate::e1: return right(side, defaultUp(side));
	case Coordinate::e2: return defaultUp(side);
	}
}

Coordinate coordinate(Direction side, Direction dir) noexcept
{
	Direction e1Dir = direction(side, Coordinate::e1);
	if (dir == e1Dir || dir == opposite(e1Dir)) return Coordinate::e1;
	Direction e2Dir = direction(side, Coordinate::e2);
	if (dir == e2Dir || dir == opposite(e2Dir)) return Coordinate::e2;
	std::cerr << "Invalid direction, can't return coordinate." << std::endl;
	std::terminate();
}

int coordinateSign(Direction side, Coordinate coord) noexcept
{
	return sgn(sfz::sum(toVector(direction(side, coord))));
}

Coordinate other(Coordinate coord) noexcept
{
	return coord != Coordinate::e1 ? Coordinate::e1 : Coordinate::e2;
}

} // namespace s3