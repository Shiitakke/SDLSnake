#include "SpriteManager.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include "Sprite.hpp"

bool SpriteManager::loadSprite(const std::string& path) {
  bool success = true;
  SDL_Surface* s = IMG_Load(path.c_str());
  if (s == nullptr) {
    printf("Failed to load surface. %s\n", IMG_GetError());
    return false;
  }

  SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
  if (t == nullptr) {
    printf("Failed to create texture from surface. %s\n", SDL_GetError());
    return false;
  }
  sprites.push_back({ t, s->w, s->h });
  sprite_map.emplace(path, n_sprites);
  ++n_sprites;
  SDL_FreeSurface(s);
  return true;
}

Sprite& SpriteManager::getSprite(const std::string& path) {
  return sprites[sprite_map[path]];
}

void SpriteManager::unloadAll() {
  sprite_map.clear();
  for (Sprite& s : sprites) {
    SDL_DestroyTexture(s.t);
  }
  sprites.clear();
  n_sprites = 0;
}