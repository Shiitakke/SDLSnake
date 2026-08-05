#pragma once

#include <string>

struct Texture;

class ITextureFactory {
public:
  virtual ~ITextureFactory() {};
  virtual Texture loadTexture(const std::string& path) = 0;
  virtual void unloadTexture(Texture& texture) = 0;
};

