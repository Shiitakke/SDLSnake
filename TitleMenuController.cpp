#include "TitleMenuController.hpp"

#include "InputSystem.hpp"
#include "SnakeGameScene.hpp"
#include "util.hpp"

void TitleMenuController::handleEvent(
  InputAction ia, 
  InputActionState state,
  InputActionValue value
) {
  switch (state) {
  case InputActionState::STARTED:
    printf("down\n");
    break;
  case InputActionState::FINISHED:
    printf("up\n");
    break;
  case InputActionState::CONTINUED:
    printf("still down\n");
    break;
  case InputActionState::RESTED:
    printf("still up\n");
    break;
  }
  if (ia == InputAction::move_up_down) {
    float value_float = std::get<float>(value);
    if (value_float > 0.1f) {
      ia = InputAction::move_down;
    }
    else if (value_float < -0.1f) {
      ia = InputAction::move_up;
    }
  }
  switch (ia) {
  case InputAction::select_item:
    if (current_selection == MenuSelection::START) {
      world.loadScene<SnakeGameScene>();
    }
    else {
      SDL_Event quit_e;
      quit_e.type = SDL_EVENT_QUIT;
      SDL_PushEvent(&quit_e);
    }
    break;
  case InputAction::move_down:
    if (current_selection == MenuSelection::START) {
      current_selection = MenuSelection::END;
      changeSelection();
    }
    break;
  case InputAction::move_up:
    if (current_selection == MenuSelection::END) {
      current_selection = MenuSelection::START;
      changeSelection();
    }
    break;
  }
}

// Moves the cursor to the new selection
void TitleMenuController::changeSelection() {
  start_pos = arrow_transform;
  next_pos = arrow_transform;
  if (current_selection == MenuSelection::START) {
    next_pos.pos.y = start_option_transform.pos.y;
  } else {
    next_pos.pos.y = quit_option_transform.pos.y;
  }
  world.timeline_manager.destroyTimelineChecked(timeline_id);
  world.timeline_manager.createTimeline({
    .length = 200,
    .on_update = [this](float delta)->void {moveArrow(delta);},
    .delta_curve = [](float delta)->float {return 4 * delta * std::pow(1.f / (1.f + delta), 2);}
  });
}

void TitleMenuController::moveArrow(float delta) {
  Transform curr_pos = lerp(start_pos, next_pos, delta);
  arrow_transform.pos = curr_pos.pos;
}