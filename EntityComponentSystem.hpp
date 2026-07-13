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
  void registerComponentType() {
    componentManagerMap.emplace(typeid(T).name(), new ComponentManager<T>);
  }

  void createEntity(u32 id) {}

  template <typename T, typename... Targs>
  void createEntity(u32 id, T component, Targs... Fargs) {
    createComponent<T>(id, component);
    createEntity(id, Fargs...);
  }

  template <typename T>
  void createComponent(u32 id, T component) {
    getComponentManager<T>().createComponent(id, component);
  }

  template <typename T>
  void destroyComponent(u32 id) {
    getComponentManager<T>().destroyComponent(id);
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
  }

  template <typename T>
  ComponentManager<T>& getComponentManager() {
    static ComponentManager<T>& component_manager = *static_cast<ComponentManager<T>*>(componentManagerMap[typeid(T).name()]);
    return component_manager;
  }

  u32 getNextId() {
    return id_manager.getNextId();
  }

  u32 returnId(u32 id) {
    id_manager.returnId(id);
  }


private:
  IdManager id_manager;
  std::unordered_map<const char*, ComponentManagerInterface*> componentManagerMap;

};

