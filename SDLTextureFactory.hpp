#pragma once

#include "ITextureFactory.hpp"

#include <optional>

struct Texture;
struct SDL_Renderer;

class SDLTextureFactory : public ITextureFactory {
public:  
  SDLTextureFactory(SDL_Renderer* _renderer);
  ~SDLTextureFactory();
  std::optional<Texture> loadTexture(const std::string& path) override;
  void unloadTexture(Texture& texture) override;
private:
  SDL_Renderer* renderer;
};

