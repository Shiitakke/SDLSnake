#include "BuyScreenObject.hpp"

#include "BuyController.hpp"
#include "InputSystem.hpp"
#include "SnakeGameScene.hpp"
#include "World.hpp"

BuyScreenObject::BuyScreenObject(
  SnakeGameScene& _scene,
  World& _world,
  vec2 _buy_screen_origin,
  vec2 _game_screen_origin
) : 
  scene(_scene),
  world(_world),
  buy_screen_origin(_buy_screen_origin),
  game_screen_origin(_game_screen_origin)
{
  IUserController* buy_controller = new BuyController(scene, world, buy_screen_origin, game_screen_origin);
  InputSystem::instance().addToCurrentContext(buy_controller);
}

BuyScreenObject::~BuyScreenObject() {
  return;
}