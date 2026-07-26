#pragma once

#include <SDL.h>

#include "common.hpp"
#include "IUserController.hpp"


// Singleton class to handle input globally
// Controllers are added to contexts, and events are handed to controllers to handle
class InputSystem {
public:
  static InputSystem& instance() {
    static InputSystem instance;
    return instance;
  };
  InputSystem(InputSystem const&) = delete;
  void operator=(InputSystem const&) = delete;

  // We want to poll all events before controllers do anything
  // so that SDL_GetKeyboardState() is properly updated
  // and all controllers can update at once

  // Queues an sdl event to be handled
  void queueEvent(SDL_Event e);
  // Processes all sdl events in the queue
  void processEvents();
  // Pushes a new 'context' on to a stack
  // Only this 'context' will receive events to handle
  // New contexts will always be empty
  void pushContext();
  // Pops the last context from the stack
  // The context underneath will still contain controllers
  void popContext();
  void clearContexts();
  // Adds a new controller to the current context
  // Returns id of the added controller
  void addToCurrentContext(IUserController* to_add);
  // Given an id reffering to a controller,
  // removes it from the current context
  void removeFromCurrentContext();
private:
  InputSystem() {};
  ~InputSystem();
  std::vector<std::vector<IUserController*>> context_stack = { {} };
  std::vector<SDL_Event> event_queue;
};

