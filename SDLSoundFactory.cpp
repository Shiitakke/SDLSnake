#include "SDLSoundFactory.hpp"

#include <SDL_mixer.h>

SDLSoundFactory::SDLSoundFactory() {}

SDLSoundFactory::~SDLSoundFactory() {}

void* SDLSoundFactory::loadSound(const std::string& path) {
  Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
  if (chunk == nullptr) {
    printf("Failed to load sfx. %s\n", Mix_GetError());
  }
  return static_cast<void*>(chunk);
}

void SDLSoundFactory::unloadSound(void* sound) {
  Mix_FreeChunk(static_cast<Mix_Chunk*>(sound));
}