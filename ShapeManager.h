#pragma once

#include "GLSL.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "Material.h"
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"

class ShapeManager
{
public:
	ShapeManager(Shape aShape, Material aMaterial, GLint pid);

	void changeShape(Shape shape);
	void changeMaterial(Material material);

	void draw(MatrixStack &MV, GLint h_MV, GLint h_pos, GLint h_nor);

	void changeTranslation(glm::vec3 change);
	void changeScale(float change);
	void changeScale(glm::vec3 change);
	void changeRotations(glm::vec3 change);
	void resetAll();
	void resetInit();
//private:
	Shape *shape;
	Material *material;
	glm::mat4 transformations;

	glm::vec3 translations;
	glm::vec3 scales;
	glm::vec3 rotations;

	glm::vec3 initTranslations;
	glm::vec3 initScales;
	glm::vec3 initRotations;

	GLint pid;
	GLint h_ka;
	GLint h_kd;
	GLint h_ks;
	GLint h_s;
	bool firstTranslate = true;
	bool firstScale = true;
	bool firstRotate = true;

	void updateTransformations();

	void translate(const glm::vec3 &trans);
	void scale(const glm::vec3 &scale);
	void scale(float size);
	void rotate(float angle, const glm::vec3 &axis);
	void reset();

};