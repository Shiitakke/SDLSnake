#include "PhysicsSystem.hpp"

#include <functional>
#include <vector>

PhysicsSystem::PhysicsSystem(
  ComponentManager<Transform>& _transforms,
  ComponentManager<CollisionBox>& _colliders
) :
  transforms(_transforms),
  colliders(_colliders)
{

}

void PhysicsSystem::update() {
  spatial_partition.clear();
  constructPartition();
	checkCollisions();
}

// Checks all collisions in the world
void PhysicsSystem::checkCollisions() {
	// Delay running callbacks since they may delete other entities
	std::vector<std::pair<std::function<void(u32)>, u32>> callbacks;
	for (i32 i = 0; i < colliders.n_components; ++i) {
		u32 entity_id = colliders.idFromIndex(i);
		CollisionBox box1 = colliders.components[i];
		Transform original_t = transforms.getComponent(entity_id);
		Transform modified_t = roundTransformToCell(original_t);
		// Get all cells to check for this collision box
		Transform lower_t = original_t;
		Transform upper_t = original_t;
		lower_t.pos = lower_t.pos - (box1.extent / 2.f);
		upper_t.pos = upper_t.pos + (box1.extent / 2.f);
		Transform lower_cell = roundTransformToCell(lower_t);
		Transform upper_cell = roundTransformToCell(upper_t);
		
		std::vector<Transform> cells_to_check;
		float x_cells = 1 + ((upper_cell.pos.x - lower_cell.pos.x) / cell_size); // number of cells to check horizontally
		float y_cells = 1 + ((upper_cell.pos.y - lower_cell.pos.y) / cell_size); // number of cells to check vertically

		for (u32 i = 0; i < y_cells; ++i) {
			for (u32 j = 0; j < x_cells; ++j) {
				Transform to_push = lower_t;
				to_push.pos.x += (cell_size * j);
				to_push.pos.y += (cell_size * i);
				cells_to_check.push_back(
					roundTransformToCell(to_push)
				);
			}
		}		

		// Now do actual collision checking
		for (const Transform& cell_to_check : cells_to_check) {
			if (spatial_partition.contains(cell_to_check.pos)) {
				for (u32 other_id : spatial_partition[cell_to_check.pos]) {
					if (entity_id == other_id) {
						continue;
					}
					Transform other_t = transforms.getComponent(other_id);
					CollisionBox box2 = colliders.getComponent(other_id);
					if (!(box1.channels & box2.channels)) {
						continue;
					}
					vec2 dist = other_t.pos - original_t.pos;
					dist.x = std::abs(dist.x);
					dist.y = std::abs(dist.y);
					if (
						((box1.extent.x + box2.extent.x) / 2.0f) > dist.x &&
						((box1.extent.y + box2.extent.y) / 2.0f) > dist.y
						) {
						// Objects are colliding, fire onCollisionBegin events for later
						// but only if they weren't colliding last frame
						if (!currently_colliding.contains(entity_id) ||
							!currently_colliding[entity_id].contains(other_id)
							)
						{
							currently_colliding[entity_id].emplace(other_id);
							currently_colliding[other_id].emplace(entity_id);
							callbacks.push_back({ box1.on_begin_collision, other_id });
							callbacks.push_back({ box2.on_begin_collision, entity_id });
						}
					}
					else {
						if (currently_colliding.contains(entity_id) &&
							currently_colliding[entity_id].contains(other_id)
							) {
							//Collision just ended
							currently_colliding[entity_id].erase(currently_colliding[entity_id].find(other_id));
							callbacks.push_back({ box1.on_end_collision, other_id });
							callbacks.push_back({ box2.on_end_collision, entity_id });
						}
					}
				}
			}
		}
	}
	for (auto callback : callbacks) {
		callback.first(callback.second);
	}
}

void PhysicsSystem::constructPartition() {
  // reconstruct spatial hash
  for (i32 i = 0; i < colliders.n_components; ++i) {
    // get transform, round to a grid cell
    u32 entity_id = colliders.idFromIndex(i);
		Transform modified_t = roundTransformToCell(transforms.getComponent(entity_id));
    // now emplace the original transform at the cell in the partition
    if (!spatial_partition.contains(modified_t.pos)) {
      spatial_partition.insert({ modified_t.pos, {entity_id} });
    } else { 
      spatial_partition[modified_t.pos].emplace(entity_id);
    }
  }
}

Transform PhysicsSystem::roundTransformToCell(Transform t) {
	t.pos.x = std::round(t.pos.x / cell_size) * cell_size;
	t.pos.y = std::round(t.pos.y / cell_size) * cell_size;
	return t;
}
