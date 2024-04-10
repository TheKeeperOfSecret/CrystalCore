#pragma once
#include "SDL.h"
#include "Components.h"

class Collision {
public:
	static bool AABB(const SDL_Rect& col1, const SDL_Rect& col2);
	static bool AABB(const Entity* e1, const Entity* e2, bool write_collision = false);
};