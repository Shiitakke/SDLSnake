#pragma once
#include "SDL.h"
#include "Sprite.hpp"

struct SpriteView {
  u32 _id;
  std::string _sprite_path;
  SDL_Rect _src_rect;
  u32 _zindex = 0;
  SDL_RendererFlip _flip_flags = SDL_FLIP_NONE;
};
