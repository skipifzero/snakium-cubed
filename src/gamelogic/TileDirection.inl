namespace s3 {

inline TileDirection opposite(TileDirection direction) noexcept
{
	switch (direction) {
	case TileDirection::UP: return TileDirection::DOWN;
	case TileDirection::DOWN: return TileDirection::UP;
	case TileDirection::LEFT: return TileDirection::RIGHT;
	case TileDirection::RIGHT: return TileDirection::LEFT;
	}
}

inline TileDirection toLeft(TileDirection from) noexcept
{
	switch (from) {
	case TileDirection::UP: return TileDirection::RIGHT;
	case TileDirection::DOWN: return TileDirection::LEFT;
	case TileDirection::LEFT: return TileDirection::UP;
	case TileDirection::RIGHT: return TileDirection::DOWN;
	}
}

inline TileDirection toRight(TileDirection from) noexcept
{
	switch (from) {
	case TileDirection::UP: return TileDirection::LEFT;
	case TileDirection::DOWN: return TileDirection::RIGHT;
	case TileDirection::LEFT: return TileDirection::DOWN;
	case TileDirection::RIGHT: return TileDirection::UP;
	}
}

inline bool isNoTurn(TileDirection from, TileDirection to) noexcept
{
	return opposite(from) == to;
}

inline bool isLeftTurn(TileDirection from, TileDirection to) noexcept
{
	return toLeft(from) == to;
}

inline bool isRightTurn(TileDirection from, TileDirection to) noexcept
{
	return toRight(from) == to;
}

} // namespace s3