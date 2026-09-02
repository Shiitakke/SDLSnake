#pragma once

#include <SDL.h>
#include <String>

#include "common.hpp"

namespace SpriteFlip { // using namespace enum over enum class for easier bitwise OR
  enum SpriteFlip {
    NONE = 0,
    HORIZONTAL = 1 << 0,
    VERTICAL = 1 << 1,
  };
}

struct SpriteColorMod {
  u8 r = 255;
  u8 g = 255;
  u8 b = 255;
};

struct Sprite {
  std::string sprite_path;
  u32 texture_id;
  SDL_Rect src_rect;
  u32 zindex = 10;
  SpriteFlip::SpriteFlip  flip_flags = SpriteFlip::NONE;
  float parallax_factor = 1.0f;
  SpriteColorMod color_mod;
};
