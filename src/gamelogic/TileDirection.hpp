#pragma once
#ifndef S3_GAMELOGIC_TILE_DIRECTION_HPP
#define S3_GAMELOGIC_TILE_DIRECTION_HPP

namespace s3 {

/**
 * @brief Enum representing a direction on a SnakeTile.
 * Needs 2 bits.
 */
enum class TileDirection : uint8_t {
	UP = 0,
	DOWN = 1,
	LEFT = 2,
	RIGHT = 3
};

inline TileDirection opposite(TileDirection direction) noexcept;

inline TileDirection toLeft(TileDirection from) noexcept;

inline TileDirection toRight(TileDirection from) noexcept;

inline bool isNoTurn(TileDirection from, TileDirection to) noexcept;

inline bool isLeftTurn(TileDirection from, TileDirection to) noexcept;

inline bool isRightTurn(TileDirection from, TileDirection to) noexcept;

} // namespace s3

#include "gamelogic/TileDirection.inl"
#endif