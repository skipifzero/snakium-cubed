#pragma once
#ifndef S3_GAMELOGIC_OBJECT_HPP
#define S3_GAMELOGIC_OBJECT_HPP

#include <cstdint>

#include "gamelogic/Position.hpp"
#include "gamelogic/SnakeTile.hpp"

namespace s3 {

using std::int32_t;

/**
 * @brief Struct used to keep track of the lifetime and properties of an object.
 * If a property has a negative value it means it is disabled.
 */
struct Object final {
	Position position;
	TileType type = TileType::EMPTY;
	int32_t life = -1;
	int32_t earlyLife = -1;
	float timeSinceCreation = 0.0f;
};

} // namespace s3
#endif