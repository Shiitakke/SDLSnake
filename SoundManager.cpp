#include "SoundManager.hpp"

#include <string>

#include "ISoundFactory.hpp";

void* SoundManager::getSound(const std::string& path) {
  return sound_map[path];
}

void SoundManager::loadSound(const std::string& path) {
  void* resource = sound_factory->loadSound(path);
  sound_map.emplace(path, resource);
}

void SoundManager::unloadAll() {
  for (auto key : sound_map) {
    unloadSound(key.first);
  }
}

void SoundManager::setSoundFactory(ISoundFactory* _sound_factory) {
  unloadAll();
  if (sound_factory != nullptr) {
    delete sound_factory;
  }
  sound_factory = _sound_factory;
}

void SoundManager::unloadSound(const std::string& path) {
  sound_factory->unloadSound(sound_map[path]);
}