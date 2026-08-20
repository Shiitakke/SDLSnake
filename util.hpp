#pragma once
#include <tuple>

#include "common.hpp"
#include "vec.hpp"
#include "Transform.hpp"

std::tuple<u8, u8, u8> hsvTorgb(double h, double s, double v);
Transform lerp(const Transform& a, const Transform& b, float delta);
vec2 lerp(const vec2& a, const vec2& b, float delta);
