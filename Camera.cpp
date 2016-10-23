//
// sueda
// October, 2014
//

#include "Camera.h"
#include "MatrixStack.h"
#include <iostream>

Camera::Camera() :
	aspect(1.0f),
	fovy(45.0f/180.0f*M_PI),
	znear(0.1f),
	zfar(1000.0f),
	rotations(0.0, 0.0),
	translations(0.0f, 0.0f, 2.0f),
	rfactor(0.02f),
	tfactor(0.005f),
	sfactor(0.005f),
	width(400.0f),
	height(400.0f),
	flyMove(false)
{
}

Camera::~Camera()
{
	
}
void Camera::toggleFlyMove()
{
	flyMove = !flyMove;
}
void Camera::mouseMoved(glm::vec2 mouse)
{
	int threshold = 100; 

	float dx = 0, dy = 0;

	if(mouse.x != mousePrev.x)
	{
		dx = mousePrev.x - mouse.x;
	}

	if(mouse.y != mousePrev.y)
	{
		dy = mousePrev.y - mouse.y;
	}
	
	if(abs(dx) < threshold || abs(dy) < threshold)
	{
		rotations.x += rfactor * dx;
		rotations.y += rfactor * dy;
	} 
	
		
	mousePrev.x = mouse.x;
	mousePrev.y = mouse.y;
}

void Camera::applyProjectionMatrix(MatrixStack *P) const
{
	P->perspective(fovy, aspect, znear, zfar);
}

//for debugging purposes
void printMatrix(glm::mat4 matrix)
{
	for(int j = 0; j < 4; j++)
	{
		for(int i = 0; i < 4; i++)
		{
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void Camera::applyCameraMatrix(MatrixStack *MV) const
{
	glm::mat4 translate = glm::translate(translations);
				
	glm::mat4 pitch = glm::rotate(rotations.y, glm::vec3(1.0,0.0,0.0));
	glm::mat4 yaw = glm::rotate(rotations.x, glm::vec3(0.0,1.0,0.0));

	glm::mat4 camera = translate * yaw * pitch;


	MV->multMatrix(glm::inverse(camera));
}

void Camera::changeTranslation(glm::vec3 change)
{
	glm::mat4 pitch = glm::rotate(rotations.y, glm::vec3(1.0,0.0,0.0));
	glm::mat4 yaw = glm::rotate(rotations.x, glm::vec3(0.0,1.0,0.0));

	glm::vec4 transform;
	//decide whether to travel based on pitch ot not
	if(flyMove)
		transform =  yaw * pitch * glm::vec4(change, 0.0);
	else
		transform = yaw * glm::vec4(change, 0.0);
		
	translations+=glm::vec3(transform);
}

void Camera::setWindowSize(float w, float h)
{
	width = w;
	height = h;
	aspect = w/h;
}