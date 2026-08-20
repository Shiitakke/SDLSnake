#pragma once
#include <cmath>

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

  void normalise() {
    float inv_mag = 1.0f / std::sqrt(std::pow(x, 2) + std::pow(y, 2));
    x = x * inv_mag;
    y = y * inv_mag;
  }
  vec2 operator+(const vec2& other) {
    return {
      x + other.x,
      y + other.y
    };
  };
  vec2 operator-(const vec2& other) {
    return {
      x - other.x,
      y - other.y
    };
  }
  vec2 operator*(float scalar) {
    return {
      x * scalar,
      y * scalar
    };
  };
};
