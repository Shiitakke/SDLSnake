#include "World.hpp"

void World::unloadScene() {
  current_scene->deinitialise(*this, this->texture_manager);
  delete current_scene;
  current_scene = nullptr;
};

void World::updateScene() {
  current_scene->update();
}

// resets everything in the world except for the camera and scene
// intended to be run while cleaning up scenes
void World::reset() {
  ecs.reset();
  texture_manager.unloadAll();
  timeline_manager.clearTimelines();
  for (IAIController* ai_controller : ai_controllers) {
    delete ai_controller;
  }
  ai_controllers.clear();
  paused = false;
}