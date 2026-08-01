#pragma once

#include <SDL.h>

#include "Sprite.hpp"

struct SpriteView {
  std::string sprite_path;
  u32 sprite_id;
  SDL_Rect src_rect;
  u32 zindex = 10;
  SDL_RendererFlip flip_flags = SDL_FLIP_NONE;
  float parallax_factor = 1.0f;
};
