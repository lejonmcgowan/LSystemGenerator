#pragma once
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class Material
{
public:
	glm::vec3 ambience;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shineConstant;
	Material();
	Material(glm::vec3 ambience, glm::vec3 diffuse, glm::vec3 specular, float shineConstant);
};