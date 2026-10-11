#pragma once

#include <SDL3/SDL_events.h>

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
    timeline_id = world.timeline_manager.createTimeline({ .length = 1000 });
  }
  void handleEvent(const SDL_Event& e);
private:
  enum class MenuSelection {
    START = 1,
    END = 2
  };

  void changeSelection();
  void moveArrow(float delta);

  MenuSelection current_selection = MenuSelection::START;
  Transform& start_option_transform;
  Transform& quit_option_transform;
  Transform& arrow_transform;
  World& world;
  u32 timeline_id = 0;
  Transform start_pos;
  Transform next_pos;
  Transform curr_pos;
};

