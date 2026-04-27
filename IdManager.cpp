#include "IdManager.hpp"

IdManager::IdManager() {
  for (int i = 0; i < 5000; ++i) {
    ids.emplace_back(i);
  }
}

u32 IdManager::getNextId() {
  u32 front = ids.front();
  ids.pop_front();
  return front;
}

void IdManager::returnId(u32 id) {
  ids.emplace_front(id);
}