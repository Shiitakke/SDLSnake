#pragma once
#include <string>
#include <tuple>

#include "common.hpp"
#include "BitmapFont.hpp"
#include "Sprite.hpp"
#include "Transform.hpp"
#include "vec.hpp"
#include "World.hpp"

std::tuple<u8, u8, u8> hsvTorgb(double h, double s, double v);
inline Transform lerp(const Transform& a, const Transform& b, float delta);
inline vec2 lerp(const vec2& a, const vec2& b, float delta);
inline float lerp(const float a, const float b, const float delta); // Should be faster than std::lerp

// Generates a word in the ecs given a source font and transform
BitmapFontWord makeBitmapWord(BitmapFont font, World& world, std::string word, Transform origin, Sprite format);

inline Transform lerp(
  const Transform& a,
  const Transform& b,
  float delta
) {
  Transform t = {
    .pos = lerp(a.pos, b.pos, delta),
    .scale = lerp(a.scale, b.scale, delta),
    .rot = std::lerp(a.rot, b.rot, delta)
  };
  return t;
}

vec2 lerp(
  const vec2& a,
  const vec2& b,
  float delta
) {
  vec2 v = {
    .x = lerp(a.x, b.x, delta),
    .y = lerp(a.y, b.y, delta)
  };
  return v;
}

float lerp(
  const float a,
  const float b,
  const float delta
) {
  return a + (b - a) * delta;
}