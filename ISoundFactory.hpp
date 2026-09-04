#pragma once

#include <string>

class ISoundFactory {
public:
  virtual ~ISoundFactory() {};
  virtual void* loadSound(const std::string& path) = 0;
  virtual void unloadSound(void* sound) = 0;
};

