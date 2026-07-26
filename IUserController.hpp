#pragma once

#include <SDL.h>

class IUserController {
public:
  virtual ~IUserController() = default;
  virtual void handleEvent(const SDL_Event& e) = 0;
};

