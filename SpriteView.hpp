#pragma once
#include "SDL.h"
#include "Sprite.hpp"

struct SpriteView {
  u32 id;
  std::string sprite_path;
  SDL_Rect src_rect;
  u32 zindex = 0;
  SDL_RendererFlip flip_flags = SDL_FLIP_NONE;
};
