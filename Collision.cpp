#include "Collision.h"
#include <iostream>

/// <summary>
/// The standard AABB function. Returns true if the rectangles intersect, and false if not.
/// </summary>
/// <param name="col1 - the first rectangle"></param>
/// <param name="col2 - the second rectangle"></param>
/// <returns></returns>
bool Collision::AABB(const SDL_Rect& col1, const SDL_Rect& col2) {
    return (col1.x + col1.w > col2.x &&
        col2.x + col2.w > col1.x &&
        col1.y + col1.h > col2.y &&
        col2.y + col2.h > col1.y);
}

/// <summary>
/// The standard AABB function. Returns true if the rectangles of the entities intersect, and false if not.
/// </summary>
/// <param name="e1 - the main entity"></param>
/// <param name="e2 - a side entity"></param>
/// <param name="write_collision - the value of the bool type, depending on which the collision will be written in the console"></param>
/// <returns></returns>
bool Collision::AABB(const Entity* e1, const Entity* e2, bool write_collision) {
    if (AABB(e1->getComponent<ColliderComponent>().collider, e2->getComponent<ColliderComponent>().collider)) {
        if (write_collision)
            std::cout << "The " + e1->getComponent<ColliderComponent>().tag + " hit: " +
            e2->getComponent<ColliderComponent>().tag << std::endl;
        return true;
    }
    return false;
}


