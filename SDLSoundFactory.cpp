#include "SDLSoundFactory.hpp"

#include <SDL3_mixer/SDL_mixer.h>

SDLSoundFactory::SDLSoundFactory() {}

SDLSoundFactory::~SDLSoundFactory() {}

void* SDLSoundFactory::loadSound(const std::string& path) {
  MIX_Audio* audio = MIX_LoadAudio(
    mixer,
    path.c_str(),
    false
  );
  if (audio == nullptr) {
    printf("Failed to load sfx. %s\n", SDL_GetError());
  }
  return static_cast<void*>(audio);
}

void SDLSoundFactory::unloadSound(void* sound) {
  MIX_DestroyAudio(static_cast<MIX_Audio*>(sound));
}