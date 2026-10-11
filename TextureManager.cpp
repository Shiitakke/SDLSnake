#include "Texturemanager.hpp"

#include "Texture.hpp"
#include "ITextureFactory.hpp"

bool TextureManager::loadTexture(const std::string& path) {
  if (texture_map.contains(path)) {
    return false;
  }

  Texture new_texture = texture_factory->loadTexture(path);
  textures.push_back(new_texture);
  texture_map.emplace(path, n_textures);
  ++n_textures;

  return true;
}

void TextureManager::unloadTexture(const std::string& path) {
  texture_factory->unloadTexture(textures[texture_map[path]]);
  texture_map.erase(texture_map.find(path));
}

Texture& TextureManager::getTexture(const std::string& path) {
  return textures[texture_map[path]];
}

Texture& TextureManager::getTexture(u32 id) {
  return textures[id];
}

u32 TextureManager::getTextureid(const std::string& path) {
  return texture_map[path];
}

void TextureManager::unloadAll() {
  texture_map.clear();
  for (Texture& texture : textures) {
    texture_factory->unloadTexture(texture);
  }
  textures.clear();
  n_textures = 0;
}