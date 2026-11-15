#pragma once
#include <cmath>
#include <numbers>

#include "common.hpp"

template<int n>
struct vec; 

template<>
struct vec<2>;
typedef vec<2> vec2;

template<>
struct vec<3>;
typedef vec<3> vec3;

template<>
struct vec<2> {
  float x = 0.f;
  float y = 0.f;

  void normalise() {
    float inv_mag = 1.0f / std::sqrt(std::pow(x, 2) + std::pow(y, 2));
    x = x * inv_mag;
    y = y * inv_mag;
  }
  vec2 rotate(double angle) {
    // make it clockwise to align with transform rotations
    angle = -(angle * (std::numbers::pi / 180));
    double cos_theta = std::cos(angle);
    double sin_theta = std::sin(angle);
    return {
      .x = float(cos_theta * x - sin_theta * y),
      .y = float(sin_theta * x + cos_theta * y)
    };
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
  vec2& operator+=(const vec2& other) {
    this->x += other.x;
    this->y += other.y;
    return *this;
  }
  vec2& operator-=(const vec2& other) {
    this->x -= other.x;
    this->y -= other.y;
    return *this;
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


template<>
struct vec<3> {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
};