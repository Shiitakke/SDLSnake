#include "SnakeController.hpp"

#include "InputSystem.hpp"
#include "SnakeObject.hpp"

void SnakeController::handleEvent(
	InputAction ia, 
	InputActionState state,
	InputActionValue value
) {
	switch (ia) {
	case InputAction::move_up:
		controlled_snake.setDirection(SnakeObject::SnakeDir::UP);
		break;
	case InputAction::move_down:
		controlled_snake.setDirection(SnakeObject::SnakeDir::DOWN);
		break;
	case InputAction::move_left:
		controlled_snake.setDirection(SnakeObject::SnakeDir::LEFT);
		break;
	case InputAction::move_right:
		controlled_snake.setDirection(SnakeObject::SnakeDir::RIGHT);
		break;
	}
}
