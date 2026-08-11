#include "SDLRenderer.hpp"
#include "util.hpp"

SDLRenderer::~SDLRenderer() {
	texture_manager.setRenderer(nullptr);
	texture_manager.unloadAll();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void SDLRenderer::render(float delta) {
	SDL_RenderClear(renderer);

	const Transform camera_transform = lerp(
		transforms.getComponent(camera.id),
		transforms.getNextComponent(camera.id),
		delta
	);

	for (u32 i = 0; i < views.n_components; ++i) {
		const Sprite& view = views.components[i];
		const Transform view_transform = lerp(
			transforms.getComponent(views.idFromIndex(i)),
			transforms.getNextComponent(views.idFromIndex(i)),
			delta
		);
		const SDL_Rect src_rect = view.src_rect;
		// Start at middle of viewport, translate by camera, then translate by position, center based on scale, multiply to scale from internal to actual size
		const SDL_Rect dst_rect = {
			(internal_screen_width / 2 -
			(camera_transform.pos.x * view.parallax_factor) + view_transform.pos.x -
			(src_rect.w * view_transform.scale.x / 2.f))
				* out_res_scale,

			(internal_screen_height / 2 +
			(camera_transform.pos.y * view.parallax_factor) - view_transform.pos.y -
			(src_rect.h * view_transform.scale.y / 2.f))
				* out_res_scale,

			src_rect.w * view_transform.scale.x * out_res_scale,
			src_rect.h * view_transform.scale.y * out_res_scale
		};
		SDL_RenderCopyEx(
			renderer,
			static_cast<SDL_Texture*>(texture_manager.getTexture(view.sprite_id).t),
			&src_rect,
			&dst_rect,
			view_transform.rot,
			NULL,
			static_cast<SDL_RendererFlip>(view.flip_flags)
		);
	}
#ifdef _DEBUG
	// Collision box rendering
	for (u32 i = 0; i < collisions.n_components; ++i) {
		const CollisionBox& box = collisions.components[i];
		const Transform box_transform = lerp(
			transforms.getComponent(collisions.idFromIndex(i)),
			transforms.getNextComponent(collisions.idFromIndex(i)),
			delta
		);
		// Start at middle of viewport, translate by camera, then translate by position, center based on scale, multiply to scale from internal to actual size
		const SDL_Rect dst_rect = {
			(internal_screen_width / 2 -
			(camera_transform.pos.x) + box_transform.pos.x -
			(box.extent.x / 2.f))
				* out_res_scale,

			(internal_screen_height / 2 +
			(camera_transform.pos.y) - box_transform.pos.y -
			(box.extent.y / 2.f))
				* out_res_scale,

			box.extent.x * out_res_scale,
			box.extent.y * out_res_scale
		};

		if (box.colliding) {
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		}
		else {
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		}
		SDL_RenderDrawRect(renderer, &dst_rect);
	}
#endif

	SDL_RenderPresent(renderer);
}

void SDLRenderer::handleWindowResize() {
	// Recalc the scaling for drawing
	// We render at an internal resolution
	// and then scale to the actual resolution
	// find the scaling factor
	i32 external_screen_width;
	i32 external_screen_height;
	SDL_GetWindowSize(window, &external_screen_width, &external_screen_height);

	// Fit to internal aspect ratio, use viewport to clip rendering
	const float screen_width_factor = static_cast<float>(external_screen_width) / internal_screen_width;
	const float screen_height_factor = static_cast<float>(external_screen_height) / internal_screen_height;
	out_res_scale = std::min(screen_width_factor, screen_height_factor);

	const u32 vp_width = internal_screen_width * out_res_scale;
	const u32 vp_height = internal_screen_height * out_res_scale;
	SDL_Rect vp = { (external_screen_width / 2) - vp_width / 2, (external_screen_height / 2) - vp_height / 2, vp_width, vp_height };
	SDL_RenderSetViewport(renderer, &vp);
}
