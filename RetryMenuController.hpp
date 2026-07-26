#pragma once

#include "IUserController.hpp"

#include "World.hpp"
#include "Transform.hpp"

class RetryMenuController : public IUserController {
public:
  RetryMenuController(
    World& _world,
    u32 _retry_logo_id,
    u32 _retry_option_id,
    u32 _retry_menu_id,
    u32 _retry_quit_id,
    u32 _arrow_id
  );
  ~RetryMenuController();
  void handleEvent(const SDL_Event& e) override;

private:
  // There HAS to be a better way to make menu controllers...
  enum class RetryMenuSelection {
    RETRY,
    MENU,
    QUIT
  };

  void changeSelection();
  void moveArrow(float delta);

  World& world;
  u32 retry_logo_id;
  u32 retry_option_id;
  u32 retry_menu_id;
  u32 retry_quit_id;
  u32 arrow_id;
  Transform& arrow_transform;
  Transform start_transform;
  Transform end_transform;
  Transform curr_transform;

  RetryMenuSelection selections[3] = {
    RetryMenuSelection::RETRY,
    RetryMenuSelection::MENU,
    RetryMenuSelection::QUIT
  };
  Transform selection_transforms[3] = {};
  i32 curr_selection = 0;
  u32 timeline_id = 0;
};

