#include <cmath>

#include "util.hpp"

#include "Transform.hpp"

Transform lerp(const Transform& a, const Transform& b, float delta) {
	return Transform{ .pos = lerp(a.pos, b.pos, delta),
										.scale = lerp(a.scale, b.scale, delta),
										.rot = std::lerp(a.rot, b.rot, delta)
									};
}


vec2 lerp(const vec2& a, const vec2& b, float delta) {
	return vec2	{	.x = std::lerp(a.x, b.x, delta),
								.y = std::lerp(a.y, b.y, delta)
							};
}