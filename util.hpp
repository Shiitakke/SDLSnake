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
Transform lerp(const Transform& a, const Transform& b, float delta);
vec2 lerp(const vec2& a, const vec2& b, float delta);
// Generates a word in the ecs given a source font and transform
BitmapFontWord makeBitmapWord(BitmapFont font, World& world, std::string word, Transform origin, Sprite format);