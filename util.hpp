#pragma once

#include "vec.hpp"
#include "Transform.hpp"

Transform lerp(const Transform& a, const Transform& b, float delta);
vec2 lerp(const vec2& a, const vec2& b, float delta);