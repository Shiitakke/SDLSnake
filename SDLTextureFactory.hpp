#pragma once

#include "ITextureFactory.hpp"

struct Texture;
struct SDL_Renderer;

class SDLTextureFactory : public ITextureFactory {
public:  
  SDLTextureFactory(SDL_Renderer* _renderer);
  ~SDLTextureFactory();
  Texture loadTexture(const std::string& path) override;
  void unloadTexture(Texture& texture) override;
private:
  SDL_Renderer* renderer;
};

