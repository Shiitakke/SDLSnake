#pragma once

#include <SDL.h>
#include <String>

#include "common.hpp"

struct Sprite {
  std::string sprite_path;
  u32 sprite_id;
  SDL_Rect src_rect;
  u32 zindex = 10;
  SDL_RendererFlip flip_flags = SDL_FLIP_NONE;
  float parallax_factor = 1.0f;
};
