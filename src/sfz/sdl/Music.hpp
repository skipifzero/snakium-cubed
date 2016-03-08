#pragma once

#include <SDL_mixer.h>

namespace sdl {


class Music final {
public:
	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Mix_Music* ptr = nullptr;

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Music() noexcept = default;
	Music(const Music&) = delete;
	Music& operator= (const Music&) = delete;

	Music(const char* path) noexcept;
	Music(Music&& other) noexcept;
	Music& operator= (Music&& other) noexcept;
	~Music() noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void play() noexcept;
};

} // namespace sdl