#include "SDLTextureFactory.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include "Texture.hpp"

Texture SDLTextureFactory::loadTexture(const std::string& path) {
  bool success = true;
  SDL_Surface* s = IMG_Load(path.c_str());
  if (s == nullptr) {
    printf("Failed to load surface. %s\n", IMG_GetError());
  }

  SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
  if (t == nullptr) {
    printf("Failed to create texture from surface. %s\n", SDL_GetError());
  }

  i32 w = s->w;
  i32 h = s->h;
  SDL_FreeSurface(s);

  return Texture{ .t = t,
                  .w = s->w,
                  .h = s->h
                };
};

void SDLTextureFactory::unloadTexture(Texture& texture) {
  SDL_DestroyTexture(static_cast<SDL_Texture*>(texture.t));
  texture.t = nullptr;
}
