#pragma once
#include "ECS.h"
#include "../Vector2D.h"

class Manifold {
public:
	Entity* B;

	Vector2D normal;
	float penetration;
};

/// <summary>
/// Returns min if value is less than min, max if value is greater than max, and value if it matches the specified range.
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="min - minimum value"></param>
/// <param name="max - maximum value"></param>
/// <param name="value - the value that will be checked"></param>
/// <returns></returns>
template<typename T>
T Clamp(const T& min, const T& max, const T& value) {
	if (value < min)
		return min;
	if (value > max)
		return max;
	return value;
}