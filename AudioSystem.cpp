#include "AudioSystem.hpp"

#include <stdio.h>
#include <string>

AudioSystem AudioSystem::_instance;

AudioSystem& AudioSystem::instance() {
  return AudioSystem::_instance;
}

void AudioSystem::playSound(const std::string& path) {
  queued_sounds.emplace(path);
}

void AudioSystem::resolve() {
  for (std::string path : queued_sounds) {
    if (Mix_PlayChannel(-1, sound_manager->getChunk(path), 0) == -1) {
      printf("Could not play audio. %s\n", Mix_GetError());
    };
  }
  queued_sounds.clear();
}

AudioSystem::AudioSystem() {
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)) {
    printf("Failed to initialise SDL mixer. %s\m", Mix_GetError());
  };
  Mix_AllocateChannels(64);
}

AudioSystem::~AudioSystem() {
  Mix_CloseAudio();
  Mix_Quit();  
}
