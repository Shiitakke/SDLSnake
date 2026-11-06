#pragma once

#include "common.hpp"

template <typename T>
struct OnComponentCreate {
  u32 id;
};

template <typename T>
struct OnComponentDestroy {
  u32 id;
};

// Used when needing to disable rendering interpolation for a given entity
struct OverridePrevState {
  u32 id;
};