#include "IdManager.hpp"

IdManager::IdManager() {
  for (int i = 0; i < 5000; ++i) {
    ids.emplace_back(i);
  }
}

u32 IdManager::getNextId() {
  u32 front = ids.front();
  ids.pop_front();
  used_ids.emplace(front);
  return front;
}

void IdManager::reset() {
  for (u32 used_id : used_ids) {
    ids.emplace_front(used_id);
  }
  used_ids.clear();
}

void IdManager::returnId(u32 id) {
  if (used_ids.contains(id)) {
    ids.emplace_front(id);
    used_ids.erase(id);
  }
}