#pragma once

#include "common.hpp"

template<int n>
struct vec; 

template<>
struct vec<2>;
typedef vec<2> vec2;

template<>
struct vec<2> {
  float _x = 0.f;
  float _y = 0.f;

  vec2 operator+(const vec2& other) {
    return {
      _x + other._x,
      _y + other._y
    };
  };
  vec2 operator*(const float& scalar) {
    return {
      _x * scalar,
      _y * scalar
    };
  };
};
