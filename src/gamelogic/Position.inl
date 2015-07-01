namespace s3 {

inline int16_t Position::coordinate(Coordinate coord) noexcept
{
	return coord == Coordinate::e1 ? e1 : e2;
}

inline void Position::setCoordinate(Coordinate coord, int16_t value) noexcept
{
	if (coord == Coordinate::e1) e1 = value;
	else e2 = value;
}

template<typename T>
inline int sgn(T value) noexcept
{
	return (value > static_cast<T>(0)) - (value < static_cast<T>(0));
}

inline Direction3D direction(Direction3D side, Coordinate coordinate) noexcept
{
	switch (coordinate) {
	case Coordinate::e1: return mapDefaultUp(side, Direction2D::RIGHT);
	case Coordinate::e2: return defaultUp(side);
	}
	/*switch (coordinate) {
	case Coordinate::e1:
		if (side == Direction3D::WEST || side == Direction3D::EAST) return Direction3D::SOUTH;
		else return Direction3D::EAST;
	case Coordinate::e2:
		if (side == Direction3D::UP || side == Direction3D::DOWN) return Direction3D::SOUTH;
		else return Direction3D::UP;
	}*/
}

inline sfz::vec3 directionVector(Direction3D side, Coordinate coordinate) noexcept
{
	return toVector(direction(side, coordinate));
}

inline Coordinate coordinate(Direction3D side, Direction3D dir) noexcept
{
	Direction3D e1Dir = direction(side, Coordinate::e1);
	if (dir == e1Dir || dir == opposite(e1Dir)) return Coordinate::e1;
	Direction3D e2Dir = direction(side, Coordinate::e2);
	if (dir == e2Dir || dir == opposite(e2Dir)) return Coordinate::e2;
	std::cerr << "Invalid direction, can't return coordinate." << std::endl;
	std::terminate();
}

inline Coordinate coordinate(Direction3D side, Direction2D dir) noexcept
{
	return coordinate(side, mapDefaultUp(side, dir));
}

inline int coordinateSign(Direction3D side, Coordinate coord) noexcept
{

	return sgn(sfz::sum(toVector(direction(side, coord))));
}

inline Coordinate other(Coordinate coord) noexcept
{
	return coord != Coordinate::e1 ? Coordinate::e1 : Coordinate::e2;
}

} // namespace s3