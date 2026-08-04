#pragma once

#include "assets.hpp"

#include "IScene.hpp"

#include "EntityComponentSystem.hpp"
#include "InputSystem.hpp"

#include "TextureManager.hpp"

#include "IUserController.hpp"
#include "TitleMenuController.hpp"

class TitleScreenScene : public IScene {
public:
  TitleScreenScene() = default;
  void initialise(World& world, TextureManager& texture_manager) override;
  void deinitialise(World& world, TextureManager& texture_manager) override;
  void update() override;
private:
  u32 snake_logo_id;
  u32 start_option_id;
  u32 quit_option_id;
  u32 arrow_id;
  u32 board_id;
};

