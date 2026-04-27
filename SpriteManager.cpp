#include "SpriteManager.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include "Renderer.hpp"
#include "Sprite.hpp"

bool SpriteManager::loadSprite(const std::string& path) {
  bool success = true;
  SDL_Surface* s = IMG_Load(path.c_str());
  if (s == nullptr) {
    printf("Failed to load surface. %s\n", IMG_GetError());
    return false;
  }

  SDL_Texture* t = SDL_CreateTextureFromSurface(Renderer::instance().r, s);
  if (t == nullptr) {
    printf("Failed to create texture from surface. %s\n", SDL_GetError());
    return false;
  }
  _sprites.push_back({t, s->w, s->h});
  _sprite_map.emplace(path, _n_sprites);
  ++_n_sprites;
  SDL_FreeSurface(s);
  return true;
}

Sprite& SpriteManager::getSprite(const std::string& path) {
  return _sprites[_sprite_map[path]];
}