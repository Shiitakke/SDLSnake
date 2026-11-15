#pragma once

#include <variant>
#include "vec.hpp"

// Modify this enum to change the available actions
enum class InputAction {
	move_up,
	move_down,
	move_left,
	move_right,
	move_left_right, // analog version of left and right
	move_up_down,
	pause,
	unpause,
	open_shop,
	close_shop,
	select_item
};

// When subscribing a controller to an event, you must also specify the state it subscribes to
enum class InputActionState {
	TRIGGERED,	// Given for continuous motion, where others dont make sense
	STARTED,		// Action not active last frame, but is now
	FINISHED,		// Action was active last frame, but isnt now
	CONTINUED,	// Action was active last frame, and is now
	RESTED			// Action not active last frame, and isnt now
};

using InputActionValue = std::variant<
	std::monostate,
	bool,		// digital, for button press
	float,	// analog, for single axis (analog triggers)
	vec2,		// two, for two axes (control sticks)
	vec3		// three, for three axes (motion controls)
>;
