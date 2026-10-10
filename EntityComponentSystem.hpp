#pragma once

#include <memory>
#include <unordered_map>
#include <optional>

#include "common.hpp"
#include "ComponentManager.hpp"
#include "IdManager.hpp"

class EntityComponentSystem {
public:
  template <typename T>
  ComponentManager<T>& registerComponentType() {
    componentManagerMap.emplace(typeid(T).name(), new ComponentManager<T>);
    return *static_cast<ComponentManager<T>*>(componentManagerMap[typeid(T).name()]);
  }

  void createEntity(u32 id) {}

  template <typename T, typename... Targs>
  void createEntity(u32 id, T component, Targs... Fargs) {
    createComponent<T>(id, component);
    createEntity(id, Fargs...);
  }

  template <typename T, typename... Targs>
  u32 createEntity(T component, Targs... Fargs) {
    u32 id = getNextId();
    createComponent<T>(id, component);
    createEntity(id, Fargs...);
    return id;
  }

  template <typename T>
  void createComponent(u32 id, T component) {
    getComponentManager<T>().createComponent(id, component);
    component_counts[id]++;
  }

  template <typename T>
  void destroyComponent(u32 id) {
    getComponentManager<T>().destroyComponent(id);
    component_counts[id]++;
  }

  template <typename T>
  T& getc(u32 id) {
    return getComponentManager<T>().getComponent(id);
  }

  Transform& getNextc(u32 id) {
    return getComponentManager<Transform>().getNextComponent(id);
  }

  template <typename T>
  bool exists(u32 id) {
    return getComponentManager<T>().exists(id);
  }

  void destroyEntity(u32 id) {
    for (auto key : componentManagerMap) {
      key.second->destroyEntity(id);
    }
    id_manager.returnId(id);
    component_counts[id] = 0;
  }

  template <typename T>
  ComponentManager<T>& getComponentManager() {
    static ComponentManager<T>& component_manager = registerComponentType<T>();
    return component_manager;
  }

  u32 getNextId() {
    return id_manager.getNextId();
  }

  void reset() {
    for (auto key : componentManagerMap) {
      key.second->reset();
    }
    id_manager.reset();
    for (i32 i = 0; i < 5000; ++i) {
      component_counts[i] = 0;
    }
  }

  u32 returnId(u32 id) {
    id_manager.returnId(id);
  }

  std::array<i32, 5000> component_counts; // tracks number of components an entity has

private:
  IdManager id_manager;
  std::unordered_map<const char*, ComponentManagerInterface*> componentManagerMap;
};

