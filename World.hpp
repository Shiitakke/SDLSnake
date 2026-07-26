#pragma once

#include "EntityComponentSystem.hpp"
#include "SpriteManager.hpp"
#include "TimelineManager.hpp"
#include "IScene.hpp"
#include "Camera.hpp"

class World {
public:
  World() = default;
  World(Camera _camera) { camera = _camera; };
  ~World() = default;

  template <typename T>
  void loadScene() {
    if (current_scene != nullptr) {
      unloadScene();
    }
    current_scene = new T;
    current_scene->initialise(*this, this->sprite_manager);
  };
  void unloadScene();

  void updateScene() {
    current_scene->update();
  }

  EntityComponentSystem ecs;
  IScene* current_scene = nullptr;
  Camera camera;
  SpriteManager sprite_manager;
  TimelineManager timeline_manager;
  bool paused = false;

private:
};
