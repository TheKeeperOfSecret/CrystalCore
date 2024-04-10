#pragma once
#include <iostream>
#include "Components.h"
#include "../Vector2D.h"
#include <vector>
#include "Collision.h"
#include <stdarg.h>
#include "../Game.h"
#include "Manifold.h"

extern Manager manager;

class PhysicsComponent : public Component {
private:
	TransformComponent* transform;
	float mass, inv_mass, restitution;

	std::vector<std::size_t> conflictingGroups;

	float acceleration;

public:
	PhysicsComponent() = default;

	~PhysicsComponent() {	}
	
	PhysicsComponent(float mass, float restitution) {
		this->mass = mass;
		
		if (mass != 0)
			inv_mass = 1 / mass;
		else
			inv_mass = 0;

		this->restitution = restitution;
	}
	
	void init() override {
		if (!entity->hasComponent<TransformComponent>()) {
			std::cout << "[PhysicsComponent] ERROR: the entity doesn't have TransformComponent!" << std::endl;
			Game::isRunning = false;
		}
		transform = &entity->getComponent<TransformComponent>();

		mass = inv_mass = restitution = 0;
	}

	void update() override {
		for (auto& g : conflictingGroups) {
			for (auto& e : manager.getGroup(g)) {
				Manifold man;
				man.B = e;
				if (this->transform->radius != 0 && e->getComponent<TransformComponent>().radius != 0) {
					if (CirclevsCircle(&man)) {
						ResolveCollision(&man);
						PositionalCorrection(&man);
					}
				}
				if (this->transform->radius == 0 && e->getComponent<TransformComponent>().radius == 0) {
					if (AABBvsAABB(&man)) {
						ResolveCollision(&man);
						PositionalCorrection(&man);
					}
				}
				if (this->transform->radius == 0 && e->getComponent<TransformComponent>().radius != 0) {
					if (AABBvsCircle(&man)) {
						ResolveCollision(&man);
						PositionalCorrection(&man);
					}
				}
				if (this->transform->radius != 0 && e->getComponent<TransformComponent>().radius == 0) {
					man.B = this->entity;
					transform = &e->getComponent<TransformComponent>();
					AABBvsCircle(&man);
					ResolveCollision(&man);
					PositionalCorrection(&man);
					transform = &this->entity->getComponent<TransformComponent>();
				}
			}
		}
	}

	void ResolveCollision(Manifold* m) {
		Vector2D rv = m->B->getComponent<TransformComponent>().velocity - transform->velocity;

		float velAlongNormal = Vector2D::DotProduct(rv, m->normal);

		if (velAlongNormal > 0)
			return;

		float e = std::min(this->restitution, m->B->getComponent<PhysicsComponent>().restitution);

		float j = -(1 + e) * velAlongNormal;
		j /= (this->inv_mass + m->B->getComponent<PhysicsComponent>().inv_mass);

		Vector2D impulse = m->normal * j;
		
		float mass_sum = this->mass + m->B->getComponent<PhysicsComponent>().mass;

		float ratio = this->mass / mass_sum;
		transform->velocity -= impulse * ratio;

		ratio = 1 - ratio;
		m->B->getComponent<TransformComponent>().velocity += impulse * ratio;
	}

	void PositionalCorrection(Manifold* m) {
		const float percent = 0.2f;
		const float slop = 0.01f;

		Vector2D correction = m->normal * (std::max(m->penetration - slop, 0.0f) / (this->inv_mass
			+ m->B->getComponent<PhysicsComponent>().inv_mass) * percent);

		transform->position -= correction * this->inv_mass;

		m->B->getComponent<TransformComponent>().position += correction * m->B->getComponent<PhysicsComponent>().inv_mass;
	}

	bool CirclevsCircle(Manifold* m) {
		Vector2D AB = m->B->getComponent<TransformComponent>().position - transform->position;

		float r = transform->radius + m->B->getComponent<TransformComponent>().radius;

		if (AB.LengthSquared() > r * r)
			return false;

		float d = AB.Length();

		if (d != 0) {
			m->penetration = r - d;
			m->normal = AB / d;
		} else {
			m->penetration = transform->radius;
			m->normal = Vector2D(1, 0);
		}

		return true;
	}
	
	bool AABBvsAABB(Manifold* m) {
		Vector2D AB = m->B->getComponent<TransformComponent>().position - transform->position;

		float a_extent = transform->width / 2.0f;
		float b_extent = m->B->getComponent<TransformComponent>().width / 2.0f;

		float x_overlap = a_extent + b_extent - std::abs(AB.x);

		if (x_overlap > 0) {
			a_extent = transform->height / 2.0f;
			b_extent = m->B->getComponent<TransformComponent>().height / 2.0f;

			float y_overlap = a_extent + b_extent - std::abs(AB.y);
			
			if (y_overlap > 0) {
				if (x_overlap > y_overlap) {
					if (AB.x < 0)
						m->normal = Vector2D(-1, 0);
					else
						m->normal = Vector2D(1, 0);
					m->penetration = x_overlap;
				} else {
					if (AB.y < 0)
						m->normal = Vector2D(0, -1);
					else
						m->normal = Vector2D(0, 1);
					m->penetration = y_overlap;
				}
				return true;
			}
		}
		return false;
	}

	bool AABBvsCircle(Manifold* m) {
		Vector2D AB = m->B->getComponent<TransformComponent>().position - transform->position;

		Vector2D closest = AB;

		float x_extent = transform->width / 2.0f;
		float y_extent = transform->height / 2.0f;

		closest.x = Clamp(-x_extent, x_extent, closest.x);
		closest.y = Clamp(-y_extent, y_extent, closest.y);

		bool inside = false;

		if (AB == closest) {
			inside = true;

			if (std::abs(AB.x) > std::abs(AB.y)) {
				if (closest.x > 0)
					closest.x = x_extent;
				else
					closest.x = -x_extent;
			}
			else {
				if (closest.y > 0)
					closest.y = y_extent;
				else
					closest.y = -y_extent;
			}
		}

		Vector2D normal = AB - closest;
		float d = normal.LengthSquared();
		float r = m->B->getComponent<TransformComponent>().radius;

		if (d > r * r && inside)
			return false;

		d = std::sqrt(d);

		if (inside)
			m->normal = Vector2D(-1 * AB.x, -1 * AB.y);
		else
			m->normal = AB;

		m->penetration = r - d;

		return true;
	}

	void setConstants(float mass, float restitution) {
		this->mass = mass;

		if (mass != 0)
			inv_mass = 1 / mass;
		else
			inv_mass = 0;

		this->restitution = restitution;
	}

	void setFreeFallAcceleration(float accel) {
		acceleration = accel / 8.9f;
	}

	void useFreeFallAcceleration() {
		if (SDL_GetTicks() )
		transform->velocity.y -= acceleration;
	}

	void setConflictingGroups(int am_groups, ...) {
		conflictingGroups.resize(am_groups);
		va_list argGroups;
		va_start(argGroups, am_groups);
		for (size_t i = 0; i < am_groups; ++i)
			conflictingGroups.push_back(va_arg(argGroups, std::size_t));
		va_end(argGroups);
	}
};