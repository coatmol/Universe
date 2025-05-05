#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

inline glm::vec3 sphericalToCartesian(float radius, float theta, float phi)
{
	float x = radius * sin(theta) * cos(phi);
	float y = radius * cos(theta);
	float z = radius * sin(theta) * sin(phi);
	return glm::vec3(x, y, z);
}