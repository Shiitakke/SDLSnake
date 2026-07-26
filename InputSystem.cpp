#include "InputSystem.hpp"

InputSystem::~InputSystem() {
  for (auto context : context_stack) {
    for (IUserController* c : context) {
      delete c;
    }
  }
  context_stack.clear();
}

// Handles an sdl event by handing it to the current context
void InputSystem::queueEvent(SDL_Event e) {
  event_queue.push_back(e);
};

void InputSystem::processEvents() {
  for (SDL_Event& e : event_queue) {
    for (IUserController* c : context_stack.back()) {
      c->handleEvent(e);
    }
  }
  event_queue.clear();
}

// Pushes a new 'context' on to a stack
// Only this 'context' will receive events to handle
// New contexts will always be empty
void InputSystem::pushContext() {
  context_stack.push_back({});
};
// Pops the last context from the stack
// The context underneath will still contain controllers
void InputSystem::popContext() {
  context_stack.pop_back();
};
// Adds a new controller to the current context
// Returns id of the added controller
void InputSystem::addToCurrentContext(IUserController* to_add) {
  (*(context_stack.rbegin())).push_back(to_add);
};
// Given an id reffering to a controller,
// removes it from the current context
void InputSystem::removeFromCurrentContext() {};