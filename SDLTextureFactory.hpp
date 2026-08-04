#pragma once

#include "ITextureFactory.hpp"

class Texture;
class SDL_Renderer;

class SDLTextureFactory : public ITextureFactory {
public:  
  SDLTextureFactory(SDL_Renderer* _renderer) :
    renderer(_renderer)
  {
    return;
  }
  ~SDLTextureFactory() = default;
  Texture loadTexture(const std::string& path) override;
  void unloadTexture(Texture& texture) override;
private:
  SDL_Renderer* renderer;
};

