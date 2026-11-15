#include "InputSystem.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_gamepad.h>
#include <algorithm>

InputSystem::InputSystem() {
  SDL_Init(SDL_INIT_GAMEPAD);

  i32 count = 0;
  SDL_JoystickID* ids = SDL_GetGamepads(&count);
  for (i32 i = 0; i < count; ++i) {
    gamepads.emplace_back(ids[i], SDL_OpenGamepad(ids[i]));
  }
}

InputSystem::~InputSystem() {
  clearContexts();
  context_stack.clear();
  for (auto gamepad : gamepads) {
    SDL_CloseGamepad(gamepad.second);
  }
}

void InputSystem::addControllerToContext(IUserController* controller) {
  context_stack.back().emplace_back(controller);
}

void InputSystem::addInputMapping(InputMap im) {
  mappings.emplace_back(im);
}

void InputSystem::clearContexts() {
  while (context_stack.size() > 0) {
    popContext();
  }
}

void InputSystem::popContext() {
  for (IUserController* c: context_stack.back()) {
    delete c;
  }
  context_stack.pop_back();
};

void InputSystem::processEvents() {
  for (const SDL_Event& e : event_queue) {
    for (InputMap& map : mappings) {
      // Get appropriate trigger
      Trigger trigger = { std::monostate() };
      InputActionValue value = { std::monostate() };
      bool trigger_active = false;
      bool motion_event = false;
      switch (e.type) {
      case SDL_EVENT_KEY_UP:
        trigger = { e.key.scancode };
        value = { false };
        trigger_active = false;
        break;
      case SDL_EVENT_KEY_DOWN:
        if (e.key.repeat) {
          break;
        }
        trigger = { e.key.scancode };
        value = { true };
        trigger_active = true;
        break;
      case SDL_EVENT_GAMEPAD_BUTTON_UP:
        trigger = { static_cast<SDL_GamepadButton>(e.gbutton.button) };
        value = { false };
        trigger_active = false;
        break;
      case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        trigger = { static_cast<SDL_GamepadButton>(e.gbutton.button) };
        value = { true };
        trigger_active = true;
        break;
      case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        trigger = { static_cast<SDL_GamepadAxis>(e.gaxis.axis) };
        value = { float( std::clamp( float(e.gaxis.value / 32767.f) , -1.0f, 1.0f) ) };
        motion_event = true;
        // Unsure how to handle 'started' or 'finished' for axis events
        break;
      default:
        trigger = { std::monostate() };
        value = { std::monostate() }; 
      }
      // Emit appropriate input action event
      if (map.action_map.contains(trigger)) {
        // get controllers first in case one pops stack
        std::vector<IUserController*> current_context = context_stack.back();
        auto ias = map.action_map.equal_range(trigger);
        for (auto it = ias.first; it != ias.second; ++it) {
          InputAction ia = (*it).second;
          // get current state of action
          bool prev_state = false;
          if (action_states.contains(ia)) {
            prev_state = action_states[ia];
          }
          InputActionState curr_state;
          if (trigger_active) {
            curr_state = prev_state ? InputActionState::CONTINUED : InputActionState::STARTED;
          } else {
            curr_state = prev_state ? InputActionState::FINISHED : InputActionState::RESTED;
          }
          if (motion_event) {
            curr_state = InputActionState::TRIGGERED;
          }
          action_states[ia] = trigger_active;

          for (IUserController* controller : current_context) {
            controller->handleEvent(ia, curr_state, value);
          }
        }
      }
    }
  }
  event_queue.clear();
}

void InputSystem::pushContext() {
  context_stack.push_back({});
};

void InputSystem::queueSDLEvent(SDL_Event e) {
  switch (e.type) {
    // handle some events instantly
    case SDL_EVENT_GAMEPAD_ADDED:
      gamepads.emplace_back(e.gdevice.which, SDL_OpenGamepad(e.gdevice.which));
      break;
    case SDL_EVENT_GAMEPAD_REMOVED:
      for (auto it = gamepads.begin(); it != gamepads.end(); ++it) {
        if ((*it).first == e.gdevice.which) {
          SDL_CloseGamepad((*it).second);
          gamepads.erase(it);
          break;
        }
      }
      break;
    default:
      event_queue.push_back(e);  
  }
};
