#include <cmath>
#include <tuple>
#include <vector>

#include "util.hpp"

#include "Transform.hpp"

Transform lerp(
  const Transform& a, 
  const Transform& b, 
  float delta
) {
	return Transform { 
    .pos = lerp(a.pos, b.pos, delta),
		.scale = lerp(a.scale, b.scale, delta),
		.rot = std::lerp(a.rot, b.rot, delta)
	};
}

vec2 lerp(
  const vec2& a, 
  const vec2& b, 
  float delta
) {
	return vec2	{	
    .x = std::lerp(a.x, b.x, delta),
		.y = std::lerp(a.y, b.y, delta)
	};
}

// h as an angle, s and v as percentages
std::tuple<u8, u8, u8> hsvTorgb(double h, double s, double v) {
  float r;
  float g;
  float b;
  float c = v * s;
  float x = c * (1 - fabs(fmod((fmod(h, 360) / 60), 2) - 1));
  float m = v - c;

  if (0 <= h && h < 60) {
    r = c;
    g = x;
    b = 0;
  }
  else if (60 <= h && h < 120) {
    r = x;
    g = c;
    b = 0;
  }
  else if (120 <= h && h < 180) {
    r = 0;
    g = c;
    b = x;
  }
  else if (180 <= h && h < 240) {
    r = 0;
    g = x;
    b = c;
  }
  else if (240 <= h && h < 300) {
    r = x;
    g = 0;
    b = c;
  }
  else if (300 <= h && h < 360) {
    r = c;
    g = 0;
    b = x;
  }
  else {
    r = 0;
    g = 0;
    b = 0;
  }

  return {
    static_cast<u8>((r + m) * 255),
    static_cast<u8>((g + m) * 255),
    static_cast<u8>((b + m) * 255),
  };
}

BitmapFontWord makeBitmapWord(
  BitmapFont font, 
  World& world, 
  std::string word, 
  Transform origin, 
  Sprite format
) {
  std::vector<u32> letter_ids;
  Transform curr_letter_transform = origin;
  u32 letter_width = font.cellw * origin.scale.x;
  curr_letter_transform.pos.x -= (letter_width * (word.size() - 1)) / 2;
  for (char c : word) {
    u32 sprite_sheet_index = font.glyph_map[c];
    i32 row = static_cast<u8>(sprite_sheet_index) / font.cols;
    i32 col = static_cast<u8>(sprite_sheet_index) % font.cols;
    Sprite letter_sprite = format;
    letter_sprite.sprite_path = font.font_path;
    letter_sprite.texture_id = world.texture_manager.getTextureid(font.font_path);
    letter_sprite.src_rect = { .x = col * font.cellw, .y = row * font.cellh, .w = font.cellw, .h = font.cellh };
    letter_ids.emplace_back(world.ecs.createEntity(
      curr_letter_transform,
      letter_sprite
    ));
    curr_letter_transform.pos.x += letter_width;
  }
  return BitmapFontWord{
    letter_ids
  };
}