#include "SDLTextureFactory.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Texture.hpp"

SDLTextureFactory::SDLTextureFactory(SDL_Renderer* _renderer) :
  renderer(_renderer)
{

}

SDLTextureFactory::~SDLTextureFactory() {
}

Texture SDLTextureFactory::loadTexture(const std::string& path) {
  bool success = true;
  SDL_Surface* s = IMG_Load(path.c_str());
  float w = 0;
  float h = 0;
  if (s == nullptr) {
    printf("Failed to load surface. %s\n", SDL_GetError());
  } else {
    w = s->w;
    h = s->h;
  }  

  SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
  if (t == nullptr) {
    printf("Failed to create texture from surface. %s\n", SDL_GetError());
  } else {
    SDL_SetTextureScaleMode(
      t,
      SDL_SCALEMODE_PIXELART
    );
  }

  SDL_DestroySurface(s);

  return Texture{ 
    .t = t,
    .w = w,
    .h = h
   };
};

void SDLTextureFactory::unloadTexture(Texture& texture) {
  SDL_DestroyTexture(static_cast<SDL_Texture*>(texture.t));
  texture.t = nullptr;
}
