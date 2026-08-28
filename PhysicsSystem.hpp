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
  void checkCollision(
    CollisionBox box1,
    CollisionBox box2
  );
  void checkCollisions();
  void checkCollisionsOld();
  void constructPartition();
  Transform roundTransformToCell(Transform t);

  std::unordered_map<vec2, std::unordered_set<u32>> spatial_partition;
  float cell_size = 64.f;
  // Tracks what is currently colliding for on collision begin and collision end events
  std::unordered_map <u32, std::unordered_set<u32>> currently_colliding;
  ComponentManager<Transform>& transforms;
  ComponentManager<CollisionBox>& colliders;
};