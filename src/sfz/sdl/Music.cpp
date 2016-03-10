#include "sfz/sdl/Music.hpp"

#include <algorithm>
#include <iostream>

namespace sdl {

// Music: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

Music::Music(const char* path) noexcept
{
	Mix_Music* tmpPtr = Mix_LoadMUS(path);
	if (tmpPtr == NULL) {
		std::cerr << "Mix_LoadMUS() failed for \"" << path << "\", error: "
		          << Mix_GetError() << std::endl;
	} else {
		this->ptr = tmpPtr;
	}
}

Music::Music(Music&& other) noexcept
{
	std::swap(this->ptr, other.ptr);
}

Music& Music::operator= (Music&& other) noexcept
{
	std::swap(this->ptr, other.ptr);
	return *this;
}

Music::~Music() noexcept
{
	if (this->ptr != nullptr) { // The documentation doesn't say anything about freeing nullptr
		//Mix_FreeMusic(ptr); // Note, if music is fading out this will block until it is complete
	}
}

// Music: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void Music::play() noexcept
{
	if (this->ptr == nullptr) return;
	// TODO: 2nd argument is number of loops. -1 is forever, 0 is 0 times.
	int res = Mix_PlayMusic(this->ptr, -1);
	if (res == -1) {
		std::cerr << "Mix_PlayMusic() failed, error: " << Mix_GetError() << std::endl;
	}
}

// Music functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void stopMusic(int fadeOutLengthMs) noexcept
{
	if (Mix_PlayingMusic()) {
		if (fadeOutLengthMs <= 0) {
			Mix_HaltMusic();
			return;
		}

		if (Mix_FadeOutMusic(fadeOutLengthMs) == 0) {
			std::cerr << "Mix_FadeOutMusic() failed, error: " << Mix_GetError() << std::endl;
		}
	}
}

} // namespace sdl