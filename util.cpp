#include "util.hpp"

#include "Transform.hpp"

Transform lerp(const Transform& a, const Transform& b, float delta) {
	Transform ret = a;
	ret.pos = {
		a.pos.x + ((b.pos.x - a.pos.x) * delta),
		a.pos.y + ((b.pos.y - a.pos.y) * delta),
	};
	ret.rot = a.rot + ((b.rot - a.rot) * delta);
	return ret;
}
