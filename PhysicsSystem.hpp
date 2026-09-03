#pragma once

#include <unordered_map>
#include <unordered_set>

#include "CollisionBox.hpp"
#include "ComponentManager.hpp"
#include "Transform.hpp"
#include "vec.hpp"

class PhysicsSystem {
public:
  PhysicsSystem(
    ComponentManager<Transform>& _transforms,
    ComponentManager<CollisionBox>& _colliders
  );
  ~PhysicsSystem() = default;
  void update();
private:
  void checkCollisions();
  void constructPartition();
  Transform roundTransformToCell(Transform t);

  std::unordered_map<vec2, std::unordered_set<u32>> spatial_partition;
  float cell_size = 64.f;
  // Tracks what is currently colliding for on collision begin and collision end events
  // Right now, entities can be deleted without this being informed
  std::unordered_map <u32, std::unordered_set<u32>> currently_colliding;
  ComponentManager<Transform>& transforms;
  ComponentManager<CollisionBox>& colliders;
};