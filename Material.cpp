#include "Material.h"

Material::Material(glm::vec3 ambience, glm::vec3 diffuse, glm::vec3 specular, float shineConstant):
ambience(ambience),
diffuse(diffuse),
specular(specular),
shineConstant(shineConstant)
{

}

Material::Material()
{
	
}