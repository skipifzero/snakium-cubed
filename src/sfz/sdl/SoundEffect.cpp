#include "sfz/sdl/SoundEffect.hpp"

#include <algorithm>
#include <iostream>

#include <sfz/Assert.hpp>

namespace sdl {

// SoundEffect: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

SoundEffect::SoundEffect(const char* path) noexcept
{
	// SDL_OpenAudio() must have been called before this

	Mix_Chunk* tmpPtr = Mix_LoadWAV(path);
	if (tmpPtr == NULL) {
		std::cerr << "Mix_LoadWAV() failed for \"" << path << "\", error: "
		          << Mix_GetError() << std::endl;
	} else {
		this->ptr = tmpPtr;
	}
}

SoundEffect::SoundEffect(SoundEffect&& other) noexcept
{
	std::swap(this->ptr, other.ptr);
}

SoundEffect& SoundEffect::operator= (SoundEffect&& other) noexcept
{
	std::swap(this->ptr, other.ptr);
	return *this;
}

SoundEffect::~SoundEffect() noexcept
{
	if (this->ptr != nullptr) { // The documentation doesn't say anything about freeing nullptr
		Mix_FreeChunk(this->ptr);
		// Do not use chunk after this without loading a new sample to it.
		// Note: It's a bad idea to free a chunk that is still being played... 
	}
}

// SoundEffect: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void SoundEffect::play() noexcept
{
	if (this->ptr == nullptr) return;
	// Channel to play on, or -1 for the first free unreserved channel. 
	Mix_PlayChannel(-1, this->ptr, 0);
	// Returns channel being played on, ignore for now
}

void SoundEffect::setVolume(float volume) noexcept
{
	sfz_assert_debug(0.0f <= volume);
	sfz_assert_debug(volume <= 1.0f);

	int volumeInt = int(std::round(volume * MIX_MAX_VOLUME));
	Mix_VolumeChunk(this->ptr, volumeInt);
}

} // namespace sdl