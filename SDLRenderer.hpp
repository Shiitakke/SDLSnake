#pragma once

#include <SDL3/SDL.h>

#include "IRenderer.hpp"

#include "common.hpp"
#include "Transform.hpp"
#include "Sprite.hpp"
#include "Camera.hpp"
#include "ComponentManager.hpp"
#include "TextureManager.hpp"
#include "SDLTextureFactory.hpp"
#include "CollisionBox.hpp"

class SDLRenderer : public IRenderer {
public:
  SDLRenderer(
    ComponentManager<Transform>& _transforms,
    ComponentManager<Sprite>& _views,
    TextureManager& _texture_manager,
    Camera& _camera,
    ComponentManager<CollisionBox>& _collisions,
    u32 _internal_window_width,
    u32 _internal_window_height
  );
  ~SDLRenderer();

  void render(float delta) override;
  void handleWindowResize();
private:
  ComponentManager<Transform>& transforms;
  ComponentManager<Sprite>& views;
  ComponentManager<CollisionBox>& collisions;
  TextureManager& texture_manager;
  Camera& camera;
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  u32 internal_screen_width;
  u32 internal_screen_height;
  float out_res_scale = 1.0f;
};

