#pragma once

class World;
class TextureManager;

class IScene {
public:
  virtual ~IScene() = default;
  virtual void update() = 0;
  virtual void initialise(World& world, TextureManager& texture_manager) = 0;
  virtual void deinitialise(World& world, TextureManager& texture_manager) = 0;
};

