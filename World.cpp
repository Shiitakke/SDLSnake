#include "World.hpp"

void World::unloadScene() {
  current_scene->deinitialise(*this, this->sprite_manager);
  current_scene = nullptr;
};