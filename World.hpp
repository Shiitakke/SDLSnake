#pragma once

#include "EntityComponentSystem.hpp"
#include "TextureManager.hpp"
#include "TimelineManager.hpp"
#include "IScene.hpp"
#include "Camera.hpp"
#include "TimerManager.hpp"

class IAIController;

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
    current_scene->initialise(*this, this->texture_manager);
  };
  void unloadScene();

  void updateScene() {
    current_scene->update();
  }

  EntityComponentSystem ecs;
  IScene* current_scene = nullptr;
  Camera camera;
  TextureManager texture_manager;
  TimelineManager timeline_manager;
  TimerManager timer_manager;
  std::vector<IAIController*> ai_controllers;
  bool paused = false;

private:
};
