#include "gamelogic/SnakeTile.hpp"

namespace s3 {

bool digesting(TileType type) noexcept
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

SnakeTile::SnakeTile(TileType type, Direction2D from, Direction2D to) noexcept
:
	type{type},
	from{from},
	to{to}
{ }

} // namespace s3