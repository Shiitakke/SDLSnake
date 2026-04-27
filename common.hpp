#pragma once
#include <cstdint>
#include <vector>

typedef uint64_t u64;
typedef uint32_t u32;
typedef int64_t i64;
typedef int32_t i32;

constexpr u32 SCREEN_WIDTH = 1000;
constexpr u32 SCREEN_HEIGHT = 1000;

constexpr float TIME_STEP = 1000. / 64;
constexpr float GAME_SPEED_SCALAR = 1.f;