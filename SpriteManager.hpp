#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "common.hpp"
#include "Sprite.hpp"

class SpriteManager {
public:
  SpriteManager() = default;
  ~SpriteManager() = default;
  bool loadSprite(const std::string& path);
  void unloadSprite(const std::string& path);
  Sprite& getSprite(const std::string& path);
  void unloadAll();
  inline void setRenderer(SDL_Renderer* _renderer) { renderer = _renderer; };
private:
  u32 n_sprites = 0;
  std::vector<Sprite> sprites = {};
  std::unordered_map<std::string, u32> sprite_map;
  SDL_Renderer* renderer = nullptr;
};

