namespace s3 {

inline Direction3D direction(Direction3D side, Coordinate coordinate) noexcept
{
	switch (coordinate) {
	case Coordinate::e1:
		if (side == Direction3D::WEST || side == Direction3D::EAST) return Direction3D::SOUTH;
		else return Direction3D::EAST;
	case Coordinate::e2:
		if (side == Direction3D::UP || side == Direction3D::DOWN) return Direction3D::SOUTH;
		else return Direction3D::UP;
	}
}

inline sfz::vec3f directionVector(Direction3D side, Coordinate coordinate) noexcept
{
	return toVector(direction(side, coordinate));
}

} // namespace s3