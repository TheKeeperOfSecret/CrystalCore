#pragma once

#include <vector>
#include <iostream>
#include <array>
#include <bitset>
#include <memory>
#include <algorithm>
#include <stdarg.h>

class Component;
class Entity;
class Manager;

using ComponentID = std::size_t;
using Group = std::size_t;

inline ComponentID getNewComponentTypeID() {
	static ComponentID lastID = 0u;
	return lastID++;
}

template<typename T>
inline ComponentID getComponentTypeID() noexcept {
	static ComponentID typeID = getNewComponentTypeID();
	return typeID;
}

constexpr std::size_t maxComponents = 32;
constexpr std::size_t maxGroups = 32;

using ComponentBitSet = std::bitset<maxComponents>;
using GroupBitSet = std::bitset<maxGroups>;

using ComponentArray = std::array<Component*, maxComponents>;

class Component {
public:
	Entity* entity;

	virtual void init() { }
	virtual void update() { }
	virtual void draw() { }

	virtual ~Component() { }
};

class Entity {
private:
	Manager& manager;
	bool active = true;
	std::vector<std::unique_ptr<Component>> components;

	ComponentArray componentArray;
	ComponentBitSet componentBitSet;
	GroupBitSet groupBitSet;
public:
	Entity(Manager& mManager) : manager(mManager) {	}

	void update() {
		for (auto& c : components) c->update();
	}

	void draw() {
		for (auto& c : components) c->draw();
	}

	/// <summary>
	/// Returns true if the entity is active, and false if not.
	/// </summary>
	/// <returns></returns>
	bool isActive() { return active; }

	/// <summary>
	/// Destroys the essence.
	/// </summary>
	void destroy() { active = false; }

	/// <summary>
	/// Returns true if the entity belongs to the specified group, and false if not.
	/// </summary>
	/// <param name="mGroup - entity group number"></param>
	/// <returns></returns>
	bool hasGroup(Group mGroup) {
		return groupBitSet[mGroup];
	}

	/// <summary>
	/// Adds an entity to the specified group.
	/// </summary>
	/// <param name="mGroup - entity group number"></param>
	void addGroup(Group mGroup);

	/// <summary>
	/// Deletes an entity from the specified group.
	/// </summary>
	/// <param name="mGroup - entity group number"></param>
	void delGroup(Group mGroup) {
		groupBitSet[mGroup] = false;
	}

	/// <summary>
	/// Returns true if the entity has a component specified in the template, and false if not.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <returns></returns>
	template<typename T>
	bool hasComponent() {
		return componentBitSet[getComponentTypeID<T>()];
	}


	/// <summary>
	/// Adds the component specified in the template to the entity.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="...TArgs"></typeparam>
	/// <param name="...mArgs - parameters of the constructor of the component to be added"></param>
	/// <returns></returns>
	template<typename T, typename... TArgs>
	T& addComponent(TArgs&&... mArgs) {
		T* c(new T(std::forward<TArgs>(mArgs)...));
		c->entity = this;
		std::unique_ptr<Component> uPtr{ c };
		components.emplace_back(std::move(uPtr));

		componentArray[getComponentTypeID<T>()] = c;
		componentBitSet[getComponentTypeID<T>()] = true;

		c->init();
		return *c;
	}

	/// <summary>
	/// Adds the component specified in the template to the entity.
	/// </summary>
	template<typename T>
	T& addComponent() {
		T* c(new T());
		c->entity = this;
		std::unique_ptr<Component> uPtr{ c };
		components.emplace_back(std::move(uPtr));

		componentArray[getComponentTypeID<T>()] = c;
		componentBitSet[getComponentTypeID<T>()] = true;

		c->init();
		return *c;
	}

	/// <summary>
	/// Returns the component specified in the template.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <returns></returns>
	template<typename T>
	T& getComponent() const {
		auto ptr(componentArray[getComponentTypeID<T>()]);
		return *static_cast<T*>(ptr);
	}
};

class Manager {
private:
	std::vector<Group> layerOrder;
	std::vector<std::unique_ptr<Entity>> entities;
	std::array<std::vector<Entity*>, maxGroups> groupedEntities;
public:
	void update() {
		for (auto& e : entities) e->update();
	}

	void draw() {
		for (auto& e : entities) e->draw();
	}

	/// <summary>
	/// Draws groups of entities in the order specified in the set Layer Order function.
	/// </summary>
	void draw_in_order() {
		for (auto& layer : layerOrder)
			for (auto& e : getGroup(layer)) e->draw();
	}

	void refresh() {
		for (auto i(0u); i < maxGroups; ++i) {
			auto& v(groupedEntities[i]);
			v.erase(std::remove_if(std::begin(v), std::end(v),
				[i](Entity* entity) {
					return !entity->isActive() || !entity->hasGroup(i);
				}), std::end(v));
		}

		entities.erase(std::remove_if(std::begin(entities), std::end(entities),
			[](const std::unique_ptr<Entity>& mEntity) {
				return !mEntity->isActive();
			}), std::end(entities));
	}

	/// <summary>
	/// Sets the sequence of rendering entity groups.
	/// </summary>
	/// <param name="layers_amount - the number of subsequent layer parameters"></param>
	/// <param name="... - the layers in the sequence in which they will be drawn"></param>
	void setLayerOrder(int layers_amount, ...) {
		layerOrder.resize(layers_amount);
		va_list argGroups;
		va_start(argGroups, layers_amount);
		for (size_t i = 0; i < layers_amount; i++) {
			layerOrder.push_back(va_arg(argGroups, Group));
		}
		va_end(argGroups);
	}

	/// <summary>
	/// Adds the specified entity to the specified entity group.
	/// </summary>
	/// <param name="mEntity - entity"></param>
	/// <param name="mGroup - entity group number"></param>
	void AddToGroup(Entity* mEntity, Group mGroup) {
		groupedEntities[mGroup].emplace_back(mEntity);
	}

	/// <summary>
	/// Returns a group of entities represented by a vector by its number.
	/// </summary>
	/// <param name="mGroup - entity group number"></param>
	/// <returns></returns>
	std::vector<Entity*>& getGroup(Group mGroup) {
		return groupedEntities[mGroup];
	}

	Entity& addEntity() {
		Entity* e = new Entity(*this);
		std::unique_ptr<Entity> uPtr{ e };
		entities.emplace_back(std::move(uPtr));
		return *e;
	}
};