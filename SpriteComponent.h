#pragma once

#include "Components.h"
#include "../TextureManager.h"
#include "SDL.h"
#include "Animation.h"
#include <map>

class SpriteComponent : public Component {
private:
	SDL_Texture* texture;
	SDL_Rect destRect, srcRect;
	TransformComponent* transform;

	bool animated = false;
	int frames = 0;
	int speed = 100;

public:
	int animIndex = 0;
	std::map<const char*, Animation> animations;
	SDL_RendererFlip spriteFlip = SDL_FLIP_NONE;

	SpriteComponent() = default;

	SpriteComponent(const char* path) {
		setTex(path);
	}

	SpriteComponent(const char* path, SDL_RendererFlip flip) {
		setTex(path);
		spriteFlip = flip;
	}

	SpriteComponent(const char* path, bool isAnimated) {
		animated = isAnimated;
		setTex(path);
	}

	SpriteComponent(const char* path, bool isAnimated, SDL_RendererFlip flip) {
		animated = isAnimated;
		setTex(path);

		spriteFlip = flip;
	}

	/// <summary>
	/// Runs a standard animation, the index of which is 0, the number of frames is 4, and the speed is 100.
	/// </summary>
	void playDefaultIdleAnim() {
		Animation idle(0, 4, 100);
		animations.emplace("idle", idle);
		Play("idle");
	}

	~SpriteComponent() {
		SDL_DestroyTexture(texture);
	}

	void init() override {
		if (!entity->hasComponent<TransformComponent>())
			entity->addComponent<TransformComponent>();
		transform = &entity->getComponent<TransformComponent>();

		srcRect.x = static_cast<int>(transform->src_pos.x);
		srcRect.y = static_cast<int>(transform->src_pos.y);
		srcRect.w = transform->width;
		srcRect.h = transform->height;
	}

	void update() override {
		if (animated) {
			srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % frames);
			srcRect.y = animIndex * transform->height;
		}

		destRect.x = static_cast<int>(transform->position.x) - Game::camera.x;
		destRect.y = static_cast<int>(transform->position.y) - Game::camera.y;
		destRect.w = transform->width * transform->scale;
		destRect.h = transform->height * transform->scale;
	}

	void draw() override {
		TextureManager::Draw(texture, srcRect, destRect, spriteFlip);
	}

	/// <summary>
	/// Sets a new texture.
	/// </summary>
	/// <param name="path - the path to the texture"></param>
	void setTex(const char* path) {
		texture = TextureManager::LoadTexture(path);
	}

	/// <summary>
	/// Starts the animation according to its name.
	/// </summary>
	/// <param name="animName - animation name"></param>
	void Play(const char* animName) {
		frames = animations[animName].frames;
		speed = animations[animName].speed;
		animIndex = animations[animName].index;
	}

	/// <summary>
	/// Adds a new animation to the animation set.
	/// </summary>
	/// <param name="name - animation name"></param>
	/// <param name="i - animation index"></param>
	/// <param name="f - number of frames"></param>
	/// <param name="s - animation speed"></param>
	void addAnimation(const char* name, int i, int f, int s) {
		animations.emplace(name, Animation(i, f, s));
	}
};