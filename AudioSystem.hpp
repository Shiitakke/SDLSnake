#pragma once

#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <unordered_set>

#include "SoundManager.hpp"

class AudioSystem {
public:
  static AudioSystem& instance();
  void setSoundManager(SoundManager& _sound_manager) {
    sound_manager = &(_sound_manager);
  };
  AudioSystem(AudioSystem const&) = delete;
  void operator=(AudioSystem const&) = delete;

  // Adds a sound to be played when resolving
  void playSound(const std::string& path);
  // Resolves, playing all sounds added via playSound();
  void resolve();

private:
  AudioSystem();
  ~AudioSystem();

  static AudioSystem _instance;
  SoundManager* sound_manager;
  std::unordered_set<std::string> queued_sounds;
};

