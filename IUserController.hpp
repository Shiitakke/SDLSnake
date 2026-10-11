#pragma once

#include <SDL3/SDL_events.h>

class IUserController {
public:
  virtual ~IUserController() = default;
  virtual void handleEvent(const SDL_Event& e) = 0;
};

