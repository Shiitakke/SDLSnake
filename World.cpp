#include "World.hpp"

void World::unloadScene() {
  current_scene->deinitialise(*this, this->texture_manager);
  delete current_scene;
  current_scene = nullptr;
};
