#include "ShapeManager.h"
#include <iostream>

using namespace std;
void printVec3(glm::vec3 vec)
{
	cout << "(" <<  vec.x << "," << vec.y << "," << vec.z << ")" << endl;
}
void ShapeManager::resetInit()
{
	firstTranslate = firstScale = firstRotate = true;
}
ShapeManager::ShapeManager(Shape aShape, Material aMaterial, GLint pid)
{
	shape = new Shape();
	*shape = aShape;
	
	material = new Material();
	*material = aMaterial;

	this->pid = pid;
	h_ka = GLSL::getUniformLocation(pid, "ka");
	h_kd = GLSL::getUniformLocation(pid, "kd");
	h_ks = GLSL::getUniformLocation(pid, "ks");
	h_s = GLSL::getUniformLocation(pid, "s");

	transformations = glm::mat4();
	translations = glm::vec3(0.0,0.0,0.0);
	scales = glm::vec3(1.0,1.0,1.0);
	rotations = glm::vec3(0.0,0.0,0.0);
}


/*transform the local matrix to be multiplied by the MV matrix at a future time.
Following functions mimic the functionality of the transformations in MatrixStack*/
void ShapeManager::translate(const glm::vec3 &trans)
{
	transformations *= glm::translate(trans);
}

void ShapeManager::scale(const glm::vec3 &scale)
{
	transformations *= glm::scale(scale);
}

void ShapeManager::scale(float s)
{
	transformations *= glm::scale(glm::vec3(s, s, s));
}

void ShapeManager::rotate(float angle, const glm::vec3 &axis)
{
	transformations *= glm::rotate(angle, axis);
}

//draws the shape with the given tranformations and lighti-ng material 
void ShapeManager::draw(MatrixStack &MV,GLint h_MV, GLint h_pos, GLint h_nor)
{
	//printVec3(translations);

	glUniform3fv(h_ka,  1, glm::value_ptr(material->ambience));
	glUniform3fv(h_kd,  1, glm::value_ptr(material->diffuse));
	glUniform3fv(h_ks, 1, glm::value_ptr(material->specular));
	glUniform1f(h_s, material->shineConstant);
	MV.pushMatrix();
		MV.multMatrix(transformations);
		glUniformMatrix4fv(h_MV, 1, GL_FALSE, glm::value_ptr(MV.topMatrix()));
		shape->draw(h_pos, h_nor);
	MV.popMatrix();
}

//changed the shape rendered
void ShapeManager::changeShape(Shape aShape)
{
	*shape = aShape;
}

//change the material used to render shape
void ShapeManager::changeMaterial(Material aMaterial)
{
	*material = aMaterial;
}

void ShapeManager::reset()
{
	transformations = glm::mat4();
}

void ShapeManager::changeTranslation(glm::vec3 change)
{
	translations.x+=change.x;
	translations.y+=change.y;
	translations.z+=change.z;
	if(firstTranslate)
	{
		initTranslations = translations;
		firstTranslate = false;
	}
	updateTransformations();
}

/*public transformation functions. Allows user to transform shape based on 
  increments of current parameters*/
void ShapeManager::changeScale(glm::vec3 change)
{
	scales.x+=change.x;
	scales.y+=change.y;
	scales.z+=change.z;
	if(firstScale)
	{
		initScales = scales;
		firstScale = false;
	}
	updateTransformations();
}

void ShapeManager::changeScale(float change)
{
	scales.x+=change;
	scales.y+=change;
	scales.z+=change;
	if(firstScale)
	{
		initScales = scales;
		firstScale = false;
	}
	updateTransformations();
}

void ShapeManager::changeRotations(glm::vec3 change)
{
	rotations+=change;
	// direction = glm::vec3(glm::rotate(rotations.x,direction) * 
	// 			glm::rotate(rotations.y,direction) * 
	// 			glm::rotate(rotations.z, direction) * glm::vec4(direction,0.0));
	if(firstRotate)
	{
		initRotations = rotations;
		firstRotate = false;
	}
	//direction = glm::vec3(sin(rotations.x),sin(rotations.y),sin(rotations.z));
	updateTransformations();
}
void ShapeManager::resetAll()
{
	scales = initScales;
	rotations = initRotations;
	translations = initTranslations;

	updateTransformations();
}
/*performed whenever a transormation is invoked
resets the transformations to a unit matrix and performs the
new tranformations*/
void ShapeManager::updateTransformations()
{
	reset();

	translate(translations);

	rotate(rotations.x, glm::vec3(1.0, 0.0, 0.0));
	rotate(rotations.y, glm::vec3(0.0, 1.0, 0.0));
	rotate(rotations.z, glm::vec3(0.0, 0.0, 1.0));

	scale(scales);

	

}

