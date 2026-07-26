#pragma once

#include <SDL.h>
#include "common.hpp"
#include "IUserController.hpp"

#include "Transform.hpp"
#include "World.hpp"

class TitleMenuController : public IUserController {
public:
  TitleMenuController(
    Transform& _start_option_transform,
    Transform& _quit_option_transform,
    Transform& _arrow_transform,
    World& _world
  ) : 
    start_option_transform(_start_option_transform),
    quit_option_transform(_quit_option_transform),
    arrow_transform(_arrow_transform),
    world(_world)
  {
  }
  void handleEvent(const SDL_Event& e);
private:
  enum class MenuSelection {
    START = 1,
    END = 2
  };

  void changeSelection();

  MenuSelection current_selection = MenuSelection::START;
  Transform& start_option_transform;
  Transform& quit_option_transform;
  Transform& arrow_transform;
  World& world;
};

