#pragma once

#include <array>
#include <unordered_map>

#include "common.hpp"
#include "Transform.hpp"
#include "SpriteView.hpp"

template<typename T>
class ComponentManager;

template <typename T>
class ComponentManager {
public:
  ComponentManager<T>() = default;
  ~ComponentManager<T>() = default;
  
  void createComponent(const u32& id, const T& component) {
    components[n_components] = component;
    id_map[id] = n_components;
    entity_map[n_components] = id;
    ++n_components;
  }
  
  T& getComponent(const u32& id) {
    return components[id_map[id]];
  }

  void destroyComponent(const u32& entity_to_remove) {
    u32 index_to_remove = id_map[entity_to_remove];
    u32 last_index = n_components - 1;
    components[index_to_remove] = components[last_index];
    u32 last_entity = entity_map[last_index];
    id_map[last_entity] = index_to_remove;
    entity_map[index_to_remove] = last_entity;
    --n_components;
  }

  u32 n_components = 0;
  std::array<T, 5000> components;
  std::array<u32, 5000> id_map; // maps entity ids to index
  std::array<u32, 5000> entity_map; // maps index to entity id
};

template <>
class ComponentManager<Transform> {
public:
  ComponentManager<Transform>() = default;
  ~ComponentManager<Transform>() = default;

  void createComponent(const u32& id, const Transform& component) {
    components[n_components] = component;
    next_transforms[n_components] = component;
    id_map[id] = n_components;
    entity_map[n_components] = id;
    ++n_components;
  }

  Transform& getComponent(const u32& id) {
    return components[id_map[id]];
  }
  Transform& getNextComponent(const u32& id) {
    return next_transforms[id_map[id]];
  }

  void destroyComponent(const u32& entity_to_remove) {
    u32 index_to_remove = id_map[entity_to_remove];
    u32 last_index = n_components - 1;
    components[index_to_remove] = components[last_index];
    next_transforms[index_to_remove] = next_transforms[last_index];
    u32 last_entity = entity_map[last_index];
    id_map[last_entity] = index_to_remove;
    entity_map[index_to_remove] = last_entity;
    --n_components;
  }

  u32 n_components = 0;
  std::array<Transform, 5000> components;
  std::array<Transform, 5000> next_transforms;
  std::array<u32, 5000> id_map; // maps entity ids to index
  std::array<u32, 5000> entity_map; // maps index to entity id
};
