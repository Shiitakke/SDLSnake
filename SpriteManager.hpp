#pragma once

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
  Sprite& getSprite(const std::string& path);
private:
  u32 _n_sprites = 0;
  std::vector<Sprite> _sprites = {};
  std::unordered_map<std::string, u32> _sprite_map;
};

