#pragma once

#include "../Game.h"
#include "Components.h"
#include <map>
#include <stdarg.h>
#include <functional>

class KeyboardController : public Component {
private:
	TransformComponent* transform;
	SpriteComponent* sprite;
public:
	std::map<SDL_KeyCode, std::pair<std::function<void()>, std::function<void()>>> function_keys;

	KeyboardController() = default;
	
	void init() override {
		if (!entity->hasComponent<TransformComponent>())
			entity->addComponent<TransformComponent>();
		transform = &entity->getComponent<TransformComponent>();

		if (!entity->hasComponent<SpriteComponent>())
			entity->addComponent<SpriteComponent>();
		sprite = &entity->getComponent<SpriteComponent>();
	}

	void update() override {
		if (Game::event.type == SDL_KEYDOWN) {
			for (auto it = function_keys.cbegin(); it != function_keys.cend(); ++it) {
				if (Game::event.key.keysym.sym == it->first)
					((it->second).first)();
			}
		}
		if (Game::event.type == SDL_KEYUP) {
			for (auto it = function_keys.cbegin(); it != function_keys.cend(); ++it) {
				if (Game::event.key.keysym.sym == it->first)
					((it->second).second)();
			}
		}
	}

	/// <summary>
	/// Sets the program closure to the specified key.
	/// </summary>
	/// <param name="quitKey - a value with the SDL_KeyCode data type that is responsible for a specific key on the keyboard"></param>
	void setQuitKey(SDL_KeyCode quitKey) {
		function_keys.emplace(quitKey, std::make_pair<std::function<void()>, std::function<void()>>([]() {
			Game::isRunning = false;
			}, []() {}));
	}

	/// <summary>
	/// Sets a new key.
	/// </summary>
	/// <param name="key - a value with the SDL_KeyCode data type that is responsible for a specific key on the keyboard"></param>
	/// <param name="key_down - the function that is called after pressing the specified key"></param>
	/// <param name="key_up - the function that is called after pressing the specified key"></param>
	void setKey(SDL_KeyCode key, void (*key_down)(), void (*key_up)()) {
		function_keys.emplace(key, std::make_pair<std::function<void()>, std::function<void()>>(*key_down, *key_up));
	}

	/// <summary>
	/// Sets a new key.
	/// </summary>
	/// <param name="key - a value with the SDL_KeyCode data type that is responsible for a specific key on the keyboard"></param>
	/// <param name="keys_down_up - a pair containing 2 functions, its first function is called after pressing the specified key, and the second after pressing the specified key"></param>
	void setKey(SDL_KeyCode key, std::pair<std::function<void()>, std::function<void()>> keys_down_up) {
		function_keys.emplace(key, keys_down_up);
	}
};