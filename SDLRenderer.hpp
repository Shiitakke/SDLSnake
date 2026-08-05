#pragma once

#include <SDL.h>

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
    ComponentManager<CollisionBox>& _collisions
  ) :
    transforms(_transforms),
    views(_views),
    texture_manager(_texture_manager),
    camera(_camera),
    collisions(_collisions)
  {
    window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, INTERNAL_WINDOW_WIDTH, INTERNAL_WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
      printf("Failed to create window. %s", SDL_GetError());
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
      printf("Failed to create renderer. %s", SDL_GetError());
    }
    texture_manager.setTextureFactory(new SDLTextureFactory(renderer));
  };
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
  u32 internal_screen_width = INTERNAL_WINDOW_WIDTH;
  u32 internal_screen_height = INTERNAL_WINDOW_HEIGHT;
  float out_res_scale = 1.0f;
};

