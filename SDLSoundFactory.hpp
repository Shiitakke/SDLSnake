#pragma once

#include <string>

#include "ISoundFactory.hpp"

class SDLSoundFactory : public ISoundFactory {
public:
  SDLSoundFactory();
  ~SDLSoundFactory();
  void* loadSound(const std::string& path) override;
  void unloadSound(void* sound) override;
private:

};

