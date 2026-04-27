#pragma once

#include <deque>

#include "common.hpp"

class IdManager {
public:
  IdManager();

  u32 getNextId();
  void returnId(u32 id);
private:
  std::deque<u32> ids;
};

