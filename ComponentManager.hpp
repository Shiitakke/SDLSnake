#pragma once

#include <array>
#include <unordered_map>
#include <optional>

#include "common.hpp"
#include "Transform.hpp"
#include "Sprite.hpp"

class ComponentManagerInterface {
public:
  virtual ~ComponentManagerInterface() = default;
  virtual void destroyEntity(u32 id) = 0;
};

template <typename T>
class ComponentManager : public ComponentManagerInterface {
public:
  ComponentManager<T>() {
    id_map.fill(-1);
    entity_map.fill(-1);
  };
  ~ComponentManager<T>() = default;

  void createComponent(u32 id, const T& component) {
    components[n_components] = component;
    id_map[id] = n_components;
    entity_map[n_components] = id;
    ++n_components;
  }

  T& getComponent(u32 id) {
    return components[id_map[id]];
  }

  bool exists(u32 id) {
    return id_map[id] != -1;
  }

  // Given an index into the components array, returns the id of the component at the index
  u32 idFromIndex(u32 index) {
    return entity_map[index];
  }

  void destroyComponent(u32 entity_to_remove) {
    u32 index_to_remove = id_map[entity_to_remove];
    u32 last_index = n_components - 1;
    components[index_to_remove] = components[last_index];
    u32 last_entity = entity_map[last_index];
    id_map[last_entity] = index_to_remove;
    entity_map[index_to_remove] = last_entity;
    --n_components;
    id_map[entity_to_remove] = -1;
  }

  void destroyEntity(u32 id) override {
    destroyComponentChecked(id);
  }

  void destroyComponentChecked(u32 id) {
    if (exists(id)) {
      destroyComponent(id);
    }
  }

  u32 n_components = 0;
  std::array<T, 5000> components;
  std::array<i32, 5000> id_map = { -1 }; // maps entity ids to index
  std::array<i32, 5000> entity_map = { -1 }; // maps index to entity id
};

template <>
class ComponentManager<Transform> : public ComponentManagerInterface {
public:
  ComponentManager<Transform>() {
    id_map.fill(-1);
    entity_map.fill(-1);
  };
  ~ComponentManager<Transform>() = default;

  void createComponent(u32 id, const Transform& component) {
    components[n_components] = component;
    next_transforms[n_components] = component;
    id_map[id] = n_components;
    entity_map[n_components] = id;
    ++n_components;
  }

  Transform& getComponent(u32 id) {
    return components[id_map[id]];
  }
  Transform& getNextComponent(u32 id) {
    return next_transforms[id_map[id]];
  }

  // Given an index into the components array, returns the id of the component at the index
  u32 idFromIndex(u32 index) {
    return entity_map[index];
  }

  void destroyComponent(u32 entity_to_remove) {
    u32 index_to_remove = id_map[entity_to_remove];
    u32 last_index = n_components - 1;
    components[index_to_remove] = components[last_index];
    next_transforms[index_to_remove] = next_transforms[last_index];
    u32 last_entity = entity_map[last_index];
    id_map[last_entity] = index_to_remove;
    entity_map[index_to_remove] = last_entity;
    --n_components;
    id_map[entity_to_remove] = -1;
  }
  
  bool exists(u32 id) {
    return id_map[id] != -1;
  }

  void destroyEntity(u32 id) override {
    destroyComponentChecked(id);
  }

  void destroyComponentChecked(u32 id) {
    if (exists(id)) {
      destroyComponent(id);
    }
  }

  virtual void destroyEntity() {}


  u32 n_components = 0;
  std::array<Transform, 5000> components;
  std::array<Transform, 5000> next_transforms;
  std::array<i32, 5000> id_map; // maps entity ids to index
  std::array<i32, 5000> entity_map; // maps index to entity id
};



template <>
class ComponentManager<Sprite> : public ComponentManagerInterface {
public:
  ComponentManager<Sprite>() {
    id_map.fill(-1);
    entity_map.fill(-1);
  };
  ~ComponentManager<Sprite>() = default;

  void createComponent(u32 id, const Sprite& component) {
    u32 i = n_components;
    while (i > 0) {
      if (components[i - 1].zindex <= component.zindex) {
        break;
      }
      else {
        components[i] = components[i - 1];
        entity_map[i] = entity_map[i - 1];
        id_map[entity_map[i]]++;
      }
      --i;
    }
    components[i] = component;
    id_map[id] = i;
    entity_map[i] = id;
    ++n_components;
  }

  Sprite& getComponent(u32 id) {
    return components[id_map[id]];
  }

  // Given an index into the components array, returns the id of the component at the index
  u32 idFromIndex(u32 index) {
    return entity_map[index];
  }

  void destroyComponent(u32 entity_to_remove) {
    u32 index_to_remove = id_map[entity_to_remove];
    id_map[entity_to_remove] = -1;
    n_components--;
    for (u32 i = index_to_remove; i < n_components; ++i) {
      components[i] = components[i+1];
      entity_map[i] = entity_map[i+1];
      id_map[entity_map[i]]--;
      
    }
  }

  bool exists(u32 id) {
    return id_map[id] != -1;
  }

  void destroyEntity(u32 id) override {
    destroyComponentChecked(id);
  }

  void destroyComponentChecked(u32 id) {
    if (exists(id)) {
      destroyComponent(id);
    }
  }

  u32 n_components = 0;
  std::array<Sprite, 5000> components; // TODO: Swap over to using a heap (faster for sorted)
  std::array<i32, 5000> id_map; // maps entity ids to index
  std::array<i32, 5000> entity_map; // maps index to entity id
};