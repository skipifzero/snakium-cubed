namespace s3 {

// 2D Direction enum
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline Direction2D opposite(Direction2D direction) noexcept
{
	switch (direction) {
	case Direction2D::UP: return Direction2D::DOWN;
	case Direction2D::DOWN: return Direction2D::UP;
	case Direction2D::LEFT: return Direction2D::RIGHT;
	case Direction2D::RIGHT: return Direction2D::LEFT;
	}
}

inline Direction2D toLeft(Direction2D from) noexcept
{
	switch (from) {
	case Direction2D::UP: return Direction2D::RIGHT;
	case Direction2D::DOWN: return Direction2D::LEFT;
	case Direction2D::LEFT: return Direction2D::UP;
	case Direction2D::RIGHT: return Direction2D::DOWN;
	}
}

inline Direction2D toRight(Direction2D from) noexcept
{
	switch (from) {
	case Direction2D::UP: return Direction2D::LEFT;
	case Direction2D::DOWN: return Direction2D::RIGHT;
	case Direction2D::LEFT: return Direction2D::DOWN;
	case Direction2D::RIGHT: return Direction2D::UP;
	}
}

inline bool isTurn(Direction2D from, Direction2D to) noexcept
{
	return from != to && from != opposite(to);
}

inline bool isLeftTurn(Direction2D from, Direction2D to) noexcept
{
	return toLeft(from) == to;
}

inline bool isRightTurn(Direction2D from, Direction2D to) noexcept
{
	return toRight(from) == to;
}

inline std::string to_string(const Direction2D& direction) noexcept
{
	switch (direction) {
	case Direction2D::UP: return std::move(std::string{"UP"});
	case Direction2D::DOWN: return std::move(std::string{"DOWN"});
	case Direction2D::LEFT: return std::move(std::string{"LEFT"});
	case Direction2D::RIGHT: return std::move(std::string{"RIGHT"});
	}
}

inline std::ostream& operator<< (std::ostream& ostream, const Direction2D& direction) noexcept
{
	return ostream << to_string(direction);
}

// 3D Direction enum
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

inline Direction3D opposite(Direction3D direction) noexcept
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

inline Direction3D up(Direction3D side, Direction3D sideRelativeUp) noexcept
{
	if (side == sideRelativeUp || side == opposite(sideRelativeUp)) {
		std::cerr << "Invalid side relative up direction." << std::endl;
		std::terminate();
	}
	return sideRelativeUp;
}

inline Direction3D down(Direction3D side, Direction3D sideRelativeUp) noexcept
{
	if (side == sideRelativeUp || side == opposite(sideRelativeUp)) {
		std::cerr << "Invalid side relative up direction." << std::endl;
		std::terminate();
	}
	return opposite(sideRelativeUp);
}

inline Direction3D left(Direction3D side, Direction3D sideRelativeUp) noexcept
{
	if (side == sideRelativeUp || side == opposite(sideRelativeUp)) {
		std::cerr << "Invalid side relative up direction." << std::endl;
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

inline Direction3D right(Direction3D side, Direction3D sideRelativeUp) noexcept
{
	return opposite(left(side, sideRelativeUp));
}

inline Direction3D map(Direction3D side, Direction3D sideRelativeUp, Direction2D dir) noexcept
{
	switch (dir) {
	case Direction2D::UP: return up(side, sideRelativeUp);
	case Direction2D::DOWN: return down(side, sideRelativeUp);
	case Direction2D::LEFT: return left(side, sideRelativeUp);
	case Direction2D::RIGHT: return right(side, sideRelativeUp);
	}
}

inline Direction3D mapDefaultUp(Direction3D side, Direction2D dir) noexcept
{
	switch (side) {
	case Direction3D::NORTH: return map(side, Direction3D::UP, dir);
	case Direction3D::SOUTH: return map(side, Direction3D::UP, dir);
	case Direction3D::WEST: return map(side, Direction3D::UP, dir);
	case Direction3D::EAST: return map(side, Direction3D::UP, dir);
	case Direction3D::UP: return map(side, Direction3D::SOUTH, dir);
	case Direction3D::DOWN: return map(side, Direction3D::SOUTH, dir);
	}
}

inline Direction2D unMap(Direction3D side, Direction3D sideRelativeUp, Direction3D dir) noexcept
{
	if (side == sideRelativeUp || side == opposite(sideRelativeUp)) {
		std::cerr << "Invalid side relative up direction." << std::endl;
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

inline Direction2D unMapDefaultUp(Direction3D side, Direction3D dir) noexcept
{
	switch (side) {
	case Direction3D::NORTH: return unMap(side, Direction3D::UP, dir);
	case Direction3D::SOUTH: return unMap(side, Direction3D::UP, dir);
	case Direction3D::WEST: return unMap(side, Direction3D::UP, dir);
	case Direction3D::EAST: return unMap(side, Direction3D::UP, dir);
	case Direction3D::UP: return unMap(side, Direction3D::SOUTH, dir);
	case Direction3D::DOWN: return unMap(side, Direction3D::SOUTH, dir);
	}
}

inline sfz::vec3f toVector(Direction3D direction) noexcept
{
	switch (direction) {
	case Direction3D::NORTH: return sfz::vec3f{0.0f, 0.0f, -1.0f};
	case Direction3D::SOUTH: return sfz::vec3f{0.0f, 0.0f, 1.0f};
	case Direction3D::WEST: return sfz::vec3f{-1.0f, 0.0f, 0.0f};
	case Direction3D::EAST: return sfz::vec3f{1.0f, 0.0f, 0.0f};
	case Direction3D::UP: return sfz::vec3f{0.0f, 1.0f, 0.0f};
	case Direction3D::DOWN: return sfz::vec3f{0.0f, -1.0f, 0.0f};
	}
}

inline std::string to_string(const Direction3D& direction) noexcept
{
	switch (direction) {
	case Direction3D::NORTH: return std::move(std::string{"NORTH"});
	case Direction3D::SOUTH: return std::move(std::string{"SOUTH"});
	case Direction3D::WEST: return std::move(std::string{"WEST"});
	case Direction3D::EAST: return std::move(std::string{"EAST"});
	case Direction3D::UP: return std::move(std::string{"UP"});
	case Direction3D::DOWN: return std::move(std::string{"DOWN"});
	}
}

inline std::ostream& operator<< (std::ostream& ostream, const Direction3D& direction) noexcept
{
	return ostream << to_string(direction);
}

} // namespace s3