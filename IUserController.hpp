#pragma once

#include <SDL3/SDL_events.h>

#include "InputAction.hpp"

class IUserController {
public:
  virtual ~IUserController() = default;
  virtual void handleEvent(
    InputAction ia, 
    InputActionState state,
    InputActionValue value
  ) = 0;
};

