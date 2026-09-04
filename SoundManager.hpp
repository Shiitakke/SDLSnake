#pragma once
#include <SDL_mixer.h>

#include <string>
#include <unordered_map>

class ISoundFactory;

class SoundManager {
public:
  SoundManager() = default;
  ~SoundManager() = default;
  void* getSound(const std::string& path);
  void loadSound(const std::string& path);
  void setSoundFactory(ISoundFactory* _sound_factory);
  void unloadAll();
  void unloadSound(const std::string& path);
private:
  std::unordered_map<std::string, void*> sound_map;
  ISoundFactory* sound_factory;
};

