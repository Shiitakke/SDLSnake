#pragma once

#include <functional>
#include <unordered_map>
#include <string>
#include <vector>

class EventSystem {
public:
  static EventSystem& instance() {
    static EventSystem instance;
    return instance;
  };
  EventSystem(EventSystem const&) = delete;
  void operator=(EventSystem const&) = delete;

  // Subscribe to an event as given by T
  template <typename T>
  void sub(std::function<void(void*)> handler) {
    if (!subscribers.contains(typeid(T).name())) {
      subscribers.emplace(
        typeid(T).name(), 
        std::vector<std::function<void(void*)>>()
      );
    }
    subscribers[typeid(T).name()].emplace_back(handler);
  };

  // Publish an event to all subscribers to event T
  template <typename T>
  void pub(T event) {
    if (subscribers.contains(typeid(T).name())) {
      for (std::function<void(void*)> handler : subscribers[typeid(T).name()]) {
        handler(static_cast<void*>(&event));
      }
    }
  };

private:
  EventSystem() {};
  ~EventSystem() {};

  std::unordered_map<const char*, std::vector<std::function<void(void*)>>> subscribers;
};

