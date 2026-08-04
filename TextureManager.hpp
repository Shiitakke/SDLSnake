#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "common.hpp"
#include "Texture.hpp"

class ITextureFactory;

class TextureManager {
public:
  TextureManager() = default;
  ~TextureManager() = default;
  bool loadTexture(const std::string& path);
  void unloadTexture(const std::string& path);
  Texture& getTexture(const std::string& path);
  Texture& getTexture(u32 id);
  u32 getTextureid(const std::string& path);
  void unloadAll();
  void setTextureFactory(ITextureFactory* _texture_factory) { texture_factory = _texture_factory; }
  void setRenderer(SDL_Renderer* _renderer) { renderer = _renderer; };
private:
  u32 n_textures = 0;
  std::vector<Texture> textures = {};
  std::unordered_map<std::string, u32> texture_map;
  ITextureFactory* texture_factory = nullptr;
  SDL_Renderer* renderer = nullptr;
};

