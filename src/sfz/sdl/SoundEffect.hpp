#pragma once

#include <SDL_mixer.h>

namespace sdl {

class SoundEffect final {
public:
	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Mix_Chunk* ptr = nullptr;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	SoundEffect() noexcept = default;
	SoundEffect(const SoundEffect&) = delete;
	SoundEffect& operator= (const SoundEffect&) = delete;

	SoundEffect(const char* path) noexcept;
	SoundEffect(SoundEffect&& other) noexcept;
	SoundEffect& operator= (SoundEffect&& other) noexcept;
	~SoundEffect() noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void play() noexcept;

	/**
	 * Sets the volume of this sound effect
	 * @param volume in the range [0, 1]
	 */
	void setVolume(float volume) noexcept;
};

} // namespace sdl