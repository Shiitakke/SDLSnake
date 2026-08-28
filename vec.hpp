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
  vec2 operator/(float divisor) {
    return (*this) * (1 / divisor);
  }
  bool operator==(const vec2& other) const {
    return (
      x == other.x &&
      y == other.y
    );
  }
};

// temporary
// stolen from: https://stackoverflow.com/questions/59060054/implement-hash-for-custom-class-c
template<> struct std::hash<vec2> {
  std::size_t operator()(vec2 const& s) const noexcept {
    std::size_t h1 = std::hash<float>{}(s.x);
    std::size_t h2 = std::hash<float>{}(s.y);
    return h1 ^ (h2 << 1);
  }
};
