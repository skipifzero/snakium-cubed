namespace s3 {

inline bool digesting(TileType type) noexcept
{
	switch (type) {
	case TileType::HEAD_DIGESTING:
	case TileType::PRE_HEAD_DIGESTING:
	case TileType::BODY_DIGESTING:
	case TileType::TAIL_DIGESTING:
		return true;
	default:
		return false;
	}
}

inline TileType SnakeTile::type(void) const noexcept
{
	return static_cast<TileType>(mBits & 0x0F);
}

inline Direction2D SnakeTile::from(void) const noexcept
{
	return static_cast<Direction2D>(((mBits >> 4) & 0x03));
}

inline Direction2D SnakeTile::to(void) const noexcept
{
	return static_cast<Direction2D>(((mBits >> 6) & 0x03));
}

inline void SnakeTile::setType(TileType type) noexcept
{
	mBits &= 0xF0; // Clear 4 lsb
	mBits |= (static_cast<uint8_t>(type) & 0x0F); // Sets 4 lsb
}

inline void SnakeTile::setFrom(Direction2D from) noexcept
{
	mBits &= 0xCF; // Clear previous from direction
	mBits |= ((static_cast<uint8_t>(from) << 4) & 0x30); // Set new from direction
}

inline void SnakeTile::setTo(Direction2D to) noexcept
{
	mBits &= 0x3F; // Clear previous to direction
	mBits |= ((static_cast<uint8_t>(to) << 6) & 0xC0); // Set new to direction
}

} // namespace s3