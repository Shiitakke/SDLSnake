#pragma once

#include <string>

class IAudioSystem {
public:
  virtual ~IAudioSystem() {};
  virtual void playSound(const std::string& path) = 0;
  virtual void resolve() = 0;
};

