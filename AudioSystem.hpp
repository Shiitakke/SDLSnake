#pragma once

#include <SDL_mixer.h>
#include <stdio.h>
#include <string>

#include "SoundManager.hpp"

class AudioSystem {
public:
  static AudioSystem& instance();
  void setSoundManager(SoundManager& _sound_manager) {
    sound_manager = &(_sound_manager);
  };
  AudioSystem(AudioSystem const&) = delete;
  void operator=(AudioSystem const&) = delete;

  void playSound(const std::string& path);

private:
  AudioSystem();
  ~AudioSystem();

  static AudioSystem _instance;
  SoundManager* sound_manager;
};

