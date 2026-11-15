#pragma once

#include <SDL3/SDL_events.h>

enum class InputAction;

class IUserController {
public:
  virtual ~IUserController() = default;
  virtual void handleEvent(InputAction ia) = 0;
};

