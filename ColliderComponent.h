#pragma once
#include "Components.h"
#include <string>
#include "../Game.h"
#include <list>
#include <stdarg.h>
#include "Collision.h"
#include <functional>

extern Manager manager;

class Collision;

class ColliderComponent : public Component {
private:
	bool haveTexture;
	SDL_Texture* tex;
	SDL_Rect srcRect, dstRect;

	std::list<std::size_t> conflictingGroups;

	TransformComponent* transform;
public:
	SDL_Rect collider;
	std::string tag;

	ColliderComponent(std::string tag, bool haveTexture = false) {
		this->tag = tag;
		this->haveTexture = haveTexture;

		if (this->haveTexture)
			setTex("assets/textures/coltex.png", 0, 0, 16, 16);
	}

	void init() override {
		if (!entity->hasComponent<TransformComponent>())
			entity->addComponent<TransformComponent>();

		transform = &entity->getComponent<TransformComponent>();

		collider = { static_cast<int>(transform->position.x), static_cast<int>(transform->position.y),
			(transform->width * transform->scale), (transform->height * transform->scale) };
		if (haveTexture)
			dstRect = { collider.x - Game::camera.x, collider.y - Game::camera.y, collider.w, collider.h };
	}

	void update() override {
		collider = { static_cast<int>(transform->position.x), static_cast<int>(transform->position.y),
			(transform->width * transform->scale), (transform->height * transform->scale) };
		
		if (haveTexture)
			dstRect = { collider.x - Game::camera.x, collider.y - Game::camera.y, collider.w, collider.h };
	}

	void draw() override {
		if (haveTexture)
			TextureManager::Draw(tex, srcRect, dstRect, SDL_FLIP_NONE);
	}

	/// <summary>
	/// Sets a new texture.
	/// </summary>
	/// <param name="tex_path - the path to the texture"></param>
	/// <param name="srcX - position X of the source"></param>
	/// <param name="srcY - the Y position of the source"></param>
	/// <param name="width - texture width"></param>
	/// <param name="height - texture height"></param>
	void setTex(const char* tex_path, int srcX, int srcY, int width, int height) {
		tex = TextureManager::LoadTexture(tex_path);
		srcRect = { srcX, srcY, width, height };
	}

	/// <summary>
	/// Stops rendering the texture.
	/// </summary>
	void stopDraw() {
		haveTexture = false;
	}

	/// <summary>
	/// Sets conflicting groups, that is, groups whose entities will undergo a collision check with an object that has a component.
	/// </summary>
	/// <param name="am_groups - the number of subsequent parameters"></param>
	/// <param name="... - any number of entity groups"></param>
	void setCollisions(int am_groups, ...) {
		conflictingGroups.resize(am_groups);
		va_list argGroups;
		va_start(argGroups, am_groups);
		for (size_t i = 0; i < am_groups; ++i)
			conflictingGroups.push_back(va_arg(argGroups, std::size_t));
		va_end(argGroups);
	}
	
	/// <summary>
	/// Checks for collisions between the main entity and entities from the groups specified in the set Collisions function. If a collision has been detected, the function specified in the parameter is called.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="func - a function with any type of return value that takes the main entity and a side entity as parameters"></param>
	/// <param name="write_collision - the value of the bool type, depending on which the collision will be written in the console (optional)"></param>
	/// <returns></returns>
	template<typename T>
	std::map<Entity*, T>& serviceCollisions(std::function<T(Entity* _main, Entity* _side)> func, bool write_collision = false) {
		std::map<Entity*, T>* _mp = new std::map<Entity*, T>();
		for (auto& g : conflictingGroups) {
			for (auto& e : manager.getGroup(g)) {
				if (e->hasComponent<ColliderComponent>() && e != this->entity) {
					if (Collision::AABB(this->entity, e, write_collision)) {
						if (typeid(decltype(func(this->entity, e))).name() == typeid(void).name())
							func(this->entity, e);
						else
							_mp->emplace(e, func(this->entity, e));
					}
				}
			}
		}
		return *std::move(_mp);
	}
};