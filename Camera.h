//
// sueda
// October, 2014
//

#pragma  once
#ifndef __Camera__
#define __Camera__

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

class MatrixStack;

class Camera
{
public:
	
	enum {
		ROTATE = 0,
		TRANSLATE,
		SCALE
	};
	
	Camera();
	virtual ~Camera();

	void setAspect(float a) { aspect = a; };
	void setRotationFactor(float f) { rfactor = f; };
	void setTranslationFactor(float f) { tfactor = f; };
	void setScaleFactor(float f) { sfactor = f; };

	void mouseMoved(glm::vec2 mouse);
	void applyProjectionMatrix(MatrixStack *P) const;
	void applyCameraMatrix(MatrixStack *MV) const;

	void update(const bool *keys, const glm::vec2 &mouse);
	void setWindowSize(float w, float h);

	void changeTranslation(glm::vec3 change);
	void toggleFlyMove();
	glm::vec3 translations;
private:
	float aspect;
	float fovy;
	float znear;
	float zfar;
	glm::vec2 rotations;
	
	float scale;
	glm::vec2 mousePrev;
	int state;
	float rfactor;
	float tfactor;
	float sfactor;
	float width;
	float height;
	bool flyMove;
};

#endif
