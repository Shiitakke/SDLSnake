#pragma once

#include "common.hpp"

template<int n>
struct vec; 

template<>
struct vec<2>;
typedef vec<2> vec2;

template<>
struct vec<2> {
  float x = 0.f;
  float y = 0.f;

  vec2 operator+(const vec2& other) {
    return {
      x + other.x,
      y + other.y
    };
  };
  vec2 operator*(const float& scalar) {
    return {
      x * scalar,
      y * scalar
    };
  };
};
