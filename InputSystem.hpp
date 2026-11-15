#pragma once

#include <SDL3/SDL_events.h>

#include <unordered_map>
#include <variant>
#include <vector>

#include "common.hpp"
#include "IUserController.hpp"

#include "vec.hpp"
#include "InputAction.hpp"

// Input action events are what controllers should eventually receive
struct InputActionEvent {
	InputAction action;
	InputActionState state;
	InputActionValue value;
};

// Using std::variant to hash multiple SDL event types in InputMap
using Trigger = std::variant <
	std::monostate,
	SDL_Scancode,
	SDL_GamepadButton,
	SDL_GamepadAxis
>;

// InputMap is the bridge between actual input and InputActionEvents
// Create an InputMappingContext, build the input map with vairous mapTo functions
// and then register it with the input system.
// Then InputSystem emits InputActionEvents for registered controllers to receive
class InputMap {
public:
	InputMap() = default;
	~InputMap() = default;

	// Keyboard events
	void mapTo(
		SDL_Scancode code,
		InputAction ia
	) {
		action_map.emplace(Trigger{ code }, ia);
	};

	void mapTo(
		SDL_GamepadButton button,
		InputAction ia
	) {
		action_map.emplace(Trigger{ button }, ia);
	};

	void mapTo(
		SDL_GamepadAxis axis,
		InputAction ia
	) {
		action_map.emplace(Trigger{ axis }, ia);
	}

	std::unordered_multimap<
		Trigger,
		InputAction
	> action_map;


private:
};

// Singleton class to handle input globally
// InputMaps are added to contexts, 
// and controllers subscribe to ActionEvents
class InputSystem {
public:
  static InputSystem& instance() {
    static InputSystem instance;
    return instance;
  };
  InputSystem(InputSystem const&) = delete;
  void operator=(InputSystem const&) = delete;
	// Adds a controller to the current context
	void addControllerToContext(IUserController* iuc);
	// Adds an InputMap to the list of active mappings
	void addInputMapping(InputMap im);
	// Clears the entire context stack
  void clearContexts();
	// Pops the top of the context stack, freeing controllers in the context
	void popContext();
	// Processes the queued SDL events
	void processEvents();	
	// Pushes a new empty context onto the context stack
	// Only controllers on top of the stack receive InputActionEvents
  void pushContext();
	// Queues an SDL event to be handled in process events
	// SDL events are queued outside of the game loop, 
	// and processed in the game loop
	// certain events such as Gamepad Added events are handled instantly
  void queueSDLEvent(SDL_Event e);

private:
  InputSystem();
  ~InputSystem();
	std::vector<SDL_Event> event_queue;
	std::vector<InputMap> mappings = {};
	std::vector<std::vector<IUserController*>> context_stack = { {} };
	std::vector<std::pair<SDL_JoystickID, SDL_Gamepad*>> gamepads;
	std::unordered_map<InputAction, bool> prev_action_states;
	std::unordered_map<InputAction, bool> action_states;
};