#pragma once

#include "IUserController.hpp"

#include "World.hpp"
#include "Transform.hpp"

class UnpauseController : public IUserController {
public:
  UnpauseController(
    World& _world,
    u32 _pause_logo_id,
    u32 _resume_option_id,
    u32 _menu_option_id,
    u32 _quit_option_id,
    u32 _arrow_id
  );
  ~UnpauseController();
  void handleEvent(const SDL_Event& e) override;

private:
  enum class RetryMenuSelection {
    RESUME,
    MENU,
    QUIT
  };

  void changeSelection();
  void moveArrow(float delta);

  World& world;
  u32 pause_logo_id;
  u32 resume_option_id;
  u32 menu_option_id;
  u32 quit_option_id;
  u32 arrow_id;
  Transform start_transform;
  Transform end_transform;

  RetryMenuSelection selections[3] = {
    RetryMenuSelection::RESUME,
    RetryMenuSelection::MENU,
    RetryMenuSelection::QUIT
  };
  Transform selection_transforms[3] = {};
  i32 curr_selection = 0;
  u32 timeline_id = 0;
};

