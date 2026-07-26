#pragma once

class World;
class SpriteManager;

class IScene {
public:
  virtual ~IScene() = default;
  virtual void update() = 0;
  virtual void initialise(World& world, SpriteManager& sprite_manager) = 0;
  virtual void deinitialise(World& world, SpriteManager& sprite_manager) = 0;
};

