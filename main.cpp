#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_image.h>

#include <stdio.h>
#include <unordered_map>
#include <unordered_set>

#include "common.hpp"
#include "util.hpp"

#include "World.hpp"
#include "assets.hpp"

#include "SnakeGameScene.hpp"
#include "TitleScreenScene.hpp"

#include "SDLRenderer.hpp"
#include "EntityComponentSystem.hpp"
#include "InputSystem.hpp"

#include "ComponentManager.hpp"
#include "TimelineManager.hpp"

#include "Transform.hpp"
#include "Sprite.hpp"
#include "SpriteAnim.hpp"
#include "LinearVelocity.hpp"
#include "AngularVelocity.hpp"
#include "CollisionBox.hpp"

bool init();
bool initSdl();
void initGame();
void close();

void updateTimelines(
	TimelineManager& timeline_manager
);
void updateAnims(
	ComponentManager<SpriteAnim>& anims,
	ComponentManager<Sprite>& sprites
);
void updateLinearVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<LinearVelocity>& linear_velocities
);
void updateAngularVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<AngularVelocity>& angular_velocities
);
void updateCollisions(
	ComponentManager<Transform>& transforms,
	ComponentManager<CollisionBox>& collision_boxes
);

// Engine stuff
bool quit = false;
World world;
IRenderer* rendering_system;

int main(int argc, char* args[]) {
	if (!init()) {
		return 1;
	}

	//Sdl loop init
	SDL_Event e;
	u64 start = SDL_GetTicks64();
	u64 end = start;
	float period = 0;
	
	while (quit == false) {

		while (SDL_PollEvent(&e)) {
			InputSystem::instance().queueEvent(e);
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
				rendering_system->handleWindowResize();
			}
		}
			
		while (period > TIME_STEP) {
			InputSystem::instance().processEvents();
			
			ComponentManager<Transform>& transforms = world.ecs.getComponentManager<Transform>();
			transforms.components = transforms.next_transforms;

			if (!world.paused) {
				world.updateScene();
			
				updateTimelines(world.timeline_manager);

				updateAnims(
					world.ecs.getComponentManager<SpriteAnim>(),
					world.ecs.getComponentManager<Sprite>()
				);
		
				updateLinearVelocities(
					world.ecs.getComponentManager<Transform>(),
					world.ecs.getComponentManager<LinearVelocity>()
				);
				updateAngularVelocities(
					world.ecs.getComponentManager<Transform>(),
					world.ecs.getComponentManager<AngularVelocity>()
				);
				updateCollisions(
					world.ecs.getComponentManager<Transform>(),
					world.ecs.getComponentManager<CollisionBox>()
				);
			}
			period -= TIME_STEP;			
		}
		rendering_system->render(period / TIME_STEP);
		end = SDL_GetTicks64();
		period += (end - start) * GAME_SPEED_SCALAR;
		start = end;
	}
	close();
	return 0;
}

void updateTimelines(
	TimelineManager& timeline_manager
) {
	for (i32 i = 0; i < timeline_manager.timelines.n_components; ++i) {
		Timeline& timeline = timeline_manager.timelines.components[i];
		timeline.curr += TIME_STEP;
		timeline.curr = std::min(timeline.curr, timeline.length);
		float timeline_delta = timeline.curr / timeline.length;
		timeline.on_update(timeline.delta_curve(timeline_delta));
		if (timeline_delta >= 1.0f) {
			timeline_manager.destroyTimeline(timeline.id);
			i--;
		}
	}
}

void updateAnims(
	ComponentManager<SpriteAnim>& anims,
	ComponentManager<Sprite>& sprites
) {
	#pragma omp parallel for
	for (i32 i = 0; i < anims.n_components; ++i) {
		SpriteAnim& anim = anims.components[i];
		anim.since_last_update += TIME_STEP * !anim.paused;
		float frametime = 1000.0f / anim.framerate;
		while (anim.since_last_update >= frametime) {
			anim.current_frame = anim.anim_graph[anim.current_frame];
			anim.since_last_update -= frametime;
		}
		sprites.getComponent(anims.idFromIndex(i)).src_rect = {
			anim.current_frame % anim.cols * anim.cellw,
			anim.current_frame / anim.cols * anim.cellh,
			anim.cellw,
			anim.cellh
		};
	}
}

void updateLinearVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<LinearVelocity>& linear_velocities
) {
	#pragma omp parallel for
	for (i32 i = 0; i < linear_velocities.n_components; ++i) {
		LinearVelocity& v = linear_velocities.components[i];
		Transform& t = transforms.getNextComponent(linear_velocities.idFromIndex(i));
		t.pos = t.pos + v.velocity;
		v.velocity = v.acceleration + v.velocity * (1 - v.friction);
	}
}

void updateAngularVelocities(
	ComponentManager<Transform>& transforms,
	ComponentManager<AngularVelocity>& angular_velocities
) {
	#pragma omp parallel for
	for (i32 i = 0; i < angular_velocities.n_components; ++i) {
		AngularVelocity& v = angular_velocities.components[i];
		Transform& t = transforms.getNextComponent(angular_velocities.idFromIndex(i));
		t.rot = t.rot + v.velocity;
		v.velocity = v.acceleration + v.velocity * (1 - v.friction);
	}
}

void updateCollisions(
	ComponentManager<Transform>& transforms,
	ComponentManager<CollisionBox>& collision_boxes
) {
	static std::unordered_map<u32, std::unordered_set<u32>> collisions; // TODO: move to class instead of a static var
	for (i32 i = 0; i < collision_boxes.n_components; ++i) {
		CollisionBox& box1 = collision_boxes.components[i];
		u32 box1_id = collision_boxes.idFromIndex(i);
		Transform& t1 = transforms.getNextComponent(box1_id);
		for (i32 j = i + 1; j < collision_boxes.n_components; ++j) {
			CollisionBox& box2 = collision_boxes.components[j];
			if (!(box1.channels & box2.channels)) {
				continue;
			}
			u32 box2_id = collision_boxes.idFromIndex(j);
			Transform& t2 = transforms.getNextComponent(collision_boxes.idFromIndex(j));

			vec2 dist = t2.pos - t1.pos;
			dist.x = std::abs(dist.x);
			dist.y = std::abs(dist.y);
			if (
				((box1.extent.x + box2.extent.x) / 2.0f) > dist.x &&
				((box1.extent.y + box2.extent.y) / 2.0f) > dist.y
				) {
				// Objects are colliding, fire onCollisionBegin events for next frame
				// but only if they weren't colliding last frame
				if	(	!collisions.contains(box1_id) ||
							!collisions[box1_id].contains(box2_id)
						) 
				{
					collisions[box1_id].emplace(box2_id);
					box1.on_begin_collision(box2_id);
					box2.on_begin_collision(box1_id);
				}
			}
			else {
				if (collisions.contains(box1_id) &&
						collisions[box1_id].contains(box2_id)
					) {
					//Collision just ended
					collisions[box1_id].erase(collisions[box1_id].find(box2_id));
					box1.on_end_collision(box2_id);
					box2.on_end_collision(box1_id);
				}
			}
		}
	}
}

bool init() {
	if (!initSdl()) {
		return false;
	}
	initGame();
	return true;
}

bool initSdl() {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("Failed to initiaze SDL. %s\n", SDL_GetError());
		return false;
	}
	i32 img_flags = IMG_INIT_PNG;
	if(!(IMG_Init(img_flags) & img_flags)) {
		printf("Failed to initialize SDL image. %s\n", IMG_GetError());
		return false;
	};
	return true;
}

void initGame() {
	u32 cameraid = world.ecs.getNextId();
	world.camera.id = cameraid;
	world.ecs.createComponent<Transform>(cameraid, { });
	rendering_system = new SDLRenderer(
		world.ecs.getComponentManager<Transform>(),
		world.ecs.getComponentManager<Sprite>(),
		world.texture_manager,
		world.camera,
		world.ecs.getComponentManager<CollisionBox>()
	);
	world.loadScene<TitleScreenScene>();
}

// Cleans up sdl
void close() {
	SDL_Quit();
}