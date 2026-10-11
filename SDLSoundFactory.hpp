#pragma once

#include <SDL3_mixer/SDL_mixer.h>

#include <string>

#include "ISoundFactory.hpp"

class SDLSoundFactory : public ISoundFactory {
public:
  SDLSoundFactory();
  ~SDLSoundFactory();
  void* loadSound(const std::string& path) override;
  void unloadSound(void* sound) override;
private:
  MIX_Mixer* mixer;
};

