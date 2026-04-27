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
    _components[_n_components] = component;
    _id_map[id] = _n_components;
    _entity_map[_n_components] = id;
    ++_n_components;
  }
  
  T& getComponent(const u32& id) {
    return _components[_id_map[id]];
  }

  void destroyComponent(const u32& entity_to_remove) {
    u32 index_to_remove = _id_map[entity_to_remove];
    u32 last_index = _n_components - 1;
    _components[index_to_remove] = _components[last_index];
    u32 last_entity = _entity_map[last_index];
    _id_map[last_entity] = index_to_remove;
    _entity_map[index_to_remove] = last_entity;
    --_n_components;
  }

  u32 _n_components = 0;
  std::array<T, 5000> _components;
  std::array<u32, 5000> _id_map; // maps entity ids to index
  std::array<u32, 5000> _entity_map; // maps index to entity id
};

template <>
class ComponentManager<Transform> {
public:
  ComponentManager<Transform>() = default;
  ~ComponentManager<Transform>() = default;

  void createComponent(const u32& id, const Transform& component) {
    _components[_n_components] = component;
    _next_transforms[_n_components] = component;
    _id_map[id] = _n_components;
    _entity_map[_n_components] = id;
    ++_n_components;
  }

  Transform& getComponent(const u32& id) {
    return _components[_id_map[id]];
  }
  Transform& getNextComponent(const u32& id) {
    return _next_transforms[_id_map[id]];
  }

  void destroyComponent(const u32& entity_to_remove) {
    u32 index_to_remove = _id_map[entity_to_remove];
    u32 last_index = _n_components - 1;
    _components[index_to_remove] = _components[last_index];
    _next_transforms[index_to_remove] = _next_transforms[last_index];
    u32 last_entity = _entity_map[last_index];
    _id_map[last_entity] = index_to_remove;
    _entity_map[index_to_remove] = last_entity;
    --_n_components;
  }

  u32 _n_components = 0;
  std::array<Transform, 5000> _components;
  std::array<Transform, 5000> _next_transforms;
  std::array<u32, 5000> _id_map; // maps entity ids to index
  std::array<u32, 5000> _entity_map; // maps index to entity id
};
//
//template <>
//class ComponentManager<SpriteView> {
//public:
//  ComponentManager<SpriteView>() = default;
//  ~ComponentManager<SpriteView>() = default;
//
//  void createComponent(const u32& id, const SpriteView& component) {
//    _components[_n_components] = component;
//    _components.insert(
//      std::upper_bound(_components.begin(), _components.end(), component),
//      component
//    );
//    _id_map[id] = _n_components;
//    _entity_map[_n_components] = id;
//    ++_n_components;
//  }
//
//  SpriteView& getComponent(const u32& id) {
//    return _components[_id_map[id]];
//  }
//
//  void destroyComponent(const u32& entity_to_remove) {
//    u32 index_to_remove = _id_map[entity_to_remove];
//    u32 last_index = _n_components - 1;
//    _components[index_to_remove] = _components[last_index];
//    u32 last_entity = _entity_map[last_index];
//    _id_map[last_entity] = index_to_remove;
//    _entity_map[index_to_remove] = last_entity;
//    --_n_components;
//  }
//
//  u32 _n_components = 0;
//  std::vector<SpriteView> _components = std::vector<SpriteView>(5000);
//  std::array<u32, 5000> _id_map; // maps entity ids to index
//  std::array<u32, 5000> _entity_map; // maps index to entity id
//};