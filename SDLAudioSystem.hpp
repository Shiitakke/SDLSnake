#pragma once

#include <string>
#include <unordered_set>

#include "IAudioSystem.hpp"

class SoundManager;

class SDLAudioSystem : public IAudioSystem {
public:
  SDLAudioSystem(
    SoundManager& _sound_maanger
  );
  ~SDLAudioSystem();
  void playSound(const std::string& path) override;
  void resolve() override;
private:
  SoundManager& sound_manager;
  std::unordered_set<std::string> sound_queue;
};

