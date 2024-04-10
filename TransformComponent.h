#pragma once

#include "Components.h"
#include "../Vector2D.h"
#include "../Game.h"

class TransformComponent : public Component {

public:
	Vector2D past_position;
	Vector2D src_pos;
	Vector2D position;
	Vector2D velocity;

	int height = 32, width = 32, scale = 1;

	float radius = 0;

	int speed = 3;
	
	// Overloads for individual textures

	TransformComponent() = default;

	TransformComponent(int sc) {
		scale = sc;
		src_pos.Zero();
	}

	TransformComponent(float x, float y) {
		position.x = x;
		position.y = y;
		src_pos.Zero();
	}

	TransformComponent(float x, float y, int sc, float srcX = 0, float srcY = 0) {
		position.x = x;
		position.y = y;
		src_pos.x = srcX;
		src_pos.y = srcY;
		scale = sc;
	}

	TransformComponent(float x, float y, int w, int h) {
		position.x = x;
		position.y = y;
		src_pos.Zero();
		width = w;
		height = h;
	}

	TransformComponent(float x, float y, int w, int h, int sc) {
		position.x = x;
		position.y = y;
		src_pos.Zero();
		width = w;
		height = h;
		scale = sc;
	}

	// Overloads for texture groups

	TransformComponent(float srcX, float srcY, float x, float y) {
		position.x = x;
		position.y = y;
		src_pos.x = srcX;
		src_pos.y = srcY;
	}

	TransformComponent(float srcX, float srcY, float x, float y, int w, int h) {
		position.x = x;
		position.y = y;
		src_pos.x = srcX;
		src_pos.y = srcY;
		width = w;
		height = h;
	}

	TransformComponent(float srcX, float srcY, float x, float y, int w, int h, int sc) {
		position.x = x;
		position.y = y;
		src_pos.x = srcX;
		src_pos.y = srcY;
		width = w;
		height = h;
		scale = sc;
	}

	// Overload for a circle
	
	TransformComponent(float x, float y, float rds, int sc) {
		position.x = x;
		position.y = y;

		scale = sc;
		radius = rds;

		width = height = 0;
		src_pos.Zero();
	}

	void init() override {
		velocity.Zero();
	}

	void update() override {
		past_position = position;

		position.x += velocity.x * speed;
		position.y += velocity.y * speed;
	}
};