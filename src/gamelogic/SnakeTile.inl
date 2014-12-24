namespace s3 {

inline TileType SnakeTile::type(void) noexcept
{
	return static_cast<TileType>(mBits & 0x0F);
}

inline TileDirection SnakeTile::from(void) noexcept
{
	return static_cast<TileDirection>(((mBits >> 4) & 0x03));
}

inline TileDirection SnakeTile::to(void) noexcept
{
	return static_cast<TileDirection>(((mBits >> 6) & 0x03));
}

inline void SnakeTile::setType(TileType type) noexcept
{
	mBits &= 0xF0; // Clear 4 lsb
	mBits |= (static_cast<uint8_t>(type) & 0x0F); // Sets 4 lsb
}

inline void SnakeTile::setFrom(TileDirection from) noexcept
{
	mBits &= 0xCF; // Clear previous from direction
	mBits |= ((static_cast<uint8_t>(from) << 4) & 0x30); // Set new from direction
}

inline void SnakeTile::setTo(TileDirection to) noexcept
{
	mBits &= 0x3F; // Clear previous to direction
	mBits |= ((static_cast<uint8_t>(to) << 6) & 0xC0); // Set new to direction
}

} // namespace s3