#pragma once

#include <deque>
#include <unordered_set>

#include "common.hpp"

class IdManager {
public:
  IdManager();

  u32 getNextId();
  void returnId(u32 id);
  void reset();
private:
  std::unordered_set<u32> used_ids;
  std::deque<u32> ids;
};

