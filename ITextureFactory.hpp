#pragma once

#include <optional>
#include <string>

struct Texture;

class ITextureFactory {
public:
  virtual ~ITextureFactory() {};
  virtual std::optional<Texture> loadTexture(const std::string& path) = 0;
  virtual void unloadTexture(Texture& texture) = 0;
};

