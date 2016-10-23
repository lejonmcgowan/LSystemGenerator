//
// sueda
// October, 2014
//

#include <iostream>

void logIt(std::string file, int line) {
	std::cout << file << ":" << line << "\n";
}

#define HERE() logIt(__FILE__, __LINE__)

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif
#ifdef __unix__
#include <GL/glut.h>
#endif
#include "globals.h"
#include "Camera.h"
#include "Shape.h"
#include "ShapeManager.h"
#include "MatrixStack.h"
#include <stdio.h>
#include "Material.h"
#include "lsystem.h"
//#include "RenderSuite.h"
#include <random>
#include <functional>

#define SPEED_LIMIT 0.2
#define SLOWING_SPEED 0.01

using namespace std;

bool keyDown[256] = {false};

float currentX, currentY, currentZ;

int time0;
glm::vec2 mouse;
Camera camera;
int level = 3;
std::random_device seed;

//setup Random Number Generators
std::default_random_engine generatorX(seed());
std::default_random_engine generatorY(seed());
std::default_random_engine generatorZ(seed ());
std::default_random_engine generatorShade(seed());
std::default_random_engine generatorShine(seed());

//setup uniform distributions, setting the range of desired generators
std::uniform_real_distribution<float> distributionX(-10.0,10.0);
std::uniform_real_distribution<float> distributionY(0.0,10.0);
std::uniform_real_distribution<float> distributionZ(-10.0,10.0);
std::uniform_real_distribution<float> distributionShade(0.0,0.5);
std::uniform_real_distribution<float> distributionShine(5.0,300.0);

//bind together generator and distribution. can be invoked with operator()
auto randomX = std::bind(distributionX, generatorX);
auto randomY = std::bind(distributionY, generatorY);
auto randomZ = std::bind(distributionZ, generatorZ);
auto randomShade = std::bind(distributionShade, generatorShade);
auto randomShine = std::bind(distributionShine, generatorShine);

bool enable = false;
/*simple light structre*/
typedef struct Light
{
	glm::vec3 position;
	float intensity;
}Light; 


//variable list of different classes/structures
std::vector<Material> materials;
std::vector<Light> lights;
vector<Shape> shapeList;
vector<ShapeManager> shapes;

LSystem *tree, *leaves, *grass;
//iterators to navigate through lists
int matIter = 0;
int selectedLight = 0;
int selectedShape = 0;
float initialRotate = 22.5;
float rotate = initialRotate;
int windChange = 0;
int maxChange = 10;
float windIncr = .1;
/*used in easing logic. speed is halfed toward SPEED_LIMIT until it is
"close enough" (when delta is satfied). */
inline bool delta(float a, float b, float delta)
{
	return (abs(a-b) < delta);
}
inline float avg(float a, float b)
{
	float avg = (a + b)/ 2.0;
	if(delta(avg, SPEED_LIMIT,0.00001f))
		return SPEED_LIMIT;
	else if(delta(avg, -SPEED_LIMIT,0.00001f))
		return -SPEED_LIMIT;
	else if(delta(avg, 0,0.00001f))
		return 0;

	return (a + b)/ 2.0;
}
inline float toRadians(float degrees)
{
	return degrees * (3.14159/180);
}

inline float toDegrees(float degrees)
{
	return degrees / (3.14159/180);
}
void lprintVec3(glm::vec3 vec, std::string label = "")
{
	cout << label << ": (" <<  vec.x << "," << vec.y << "," << vec.z << ")" << endl;
}

/*LSystem functions
	"F" = forward() : Move forward a step of length d. A line segment between points (X,Y,Z) and (X',Y',Z') is drawn.
	"[" and "]" = push/pop(): bracket - push and pop the current state , in this project it is used to generate the tree branchs.
	"+" = turnRight(): Turn left by angle Delta, Using rotation matrix R_U(Delta).
	"-"  = turnLeft(): Turn right by angle Delta, Using rotation matrix R_U(-Delta).
	"&" = pitchUp(): Pitch down by angle Delta, Using rotation matrix R_L(Delta).
	"^"  = pitchDown(): Pitch up by angle Delta, Using rotation matrix R_L(-Delta).
	"<"  = rollLeft(): Roll left by angle Delta, Using rotation matrix R_H(Delta).
	">"  = rollRight(): Roll right by angle Delta, Using rotation matrix R_H(-Delta).
	" | " = reverse(): Turn around, Using rotation matrix R_H(180).
*/

void forward(ShapeManager& shape, MatrixStack& MV)
{
	shape.changeTranslation(glm::vec3(shape.transformations * glm::vec4(0.0f,0.0f,-1.0f,0.0f)));
	shape.draw(MV, h_MV, h_vertPos, h_vertNor);
}
void turnRight(ShapeManager& shape, MatrixStack& MV)
{
	shape.changeRotations(glm::vec3(toRadians(-rotate),0.0f,0.0f));
}
void turnLeft(ShapeManager& shape, MatrixStack& MV)
{
	shape.changeRotations(glm::vec3(toRadians(rotate),0.0f,0.0f));
}
void pitchUp(ShapeManager& shape, MatrixStack& MV)
{
	shape.changeRotations(glm::vec3(0.0f,toRadians(-rotate),0.0f));
}
void  pitchDown(ShapeManager& shape, MatrixStack& MV)
{
	shape.changeRotations(glm::vec3(0.0f,toRadians(rotate),0.0f));
}
void rollRight(ShapeManager& shape, MatrixStack& MV)
{
	shape.changeRotations(glm::vec3(0.0f,0.0f,toRadians(-rotate)));
}
void rollLeft(ShapeManager& shape, MatrixStack& MV)
{
	shape.changeRotations(glm::vec3(0.0f,0.0f,toRadians(rotate)));
}
void reverse(ShapeManager& shape, MatrixStack& MV)
{
	shape.changeRotations(glm::vec3(toRadians(180), toRadians(180), toRadians(180)));	
}
void push(ShapeManager& shape, MatrixStack& MV)
{
	tree->pushState();
	shape.changeScale(glm::vec3(-0.2f,-0.2f,-0.3f));
}

void pop(ShapeManager& shape, MatrixStack& MV)
{
	tree->popState();
}
/*LSystem functions*/
void nothing(ShapeManager& shape, MatrixStack& MV)
{
}


void loadScene(std::string file, int index)
{
	shapeList[index].load(file);
}

void setPid(GLuint& pid, std::string shader)
{
	// Create shader handles
	string vShaderName = "lab7_vert.glsl";
	string fShaderName = shader;
	GLint rc;
	GLuint VS = glCreateShader(GL_VERTEX_SHADER);
	GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);
	
	// Read shader sources
	const char *vshader = GLSL::textFileRead(vShaderName.c_str());
	const char *fshader = GLSL::textFileRead(fShaderName.c_str());
	glShaderSource(VS, 1, &vshader, NULL);
	glShaderSource(FS, 1, &fshader, NULL);
	
	// Compile vertex shader
	glCompileShader(VS);
	GLSL::printError();
	glGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
	GLSL::printShaderInfoLog(VS);
	if(!rc) {
		printf("Error compiling vertex shader %s\n", vShaderName.c_str());
	}
	
	// Compile fragment shader
	glCompileShader(FS);
	GLSL::printError();
	glGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
	GLSL::printShaderInfoLog(FS);
	if(!rc) {
		printf("Error compiling fragment shader %s\n", fShaderName.c_str());
	}
	
	// Create the program and link
	pid = glCreateProgram();
	glAttachShader(pid, VS);
	glAttachShader(pid, FS);
	glLinkProgram(pid);
	GLSL::printError();
	glGetProgramiv(pid, GL_LINK_STATUS, &rc);
	GLSL::printProgramInfoLog(pid);
	if(!rc) {
		printf("Error linking shaders %s and %s\n", vShaderName.c_str(), fShaderName.c_str());
	}
	
	// Check GLSL
	GLSL::checkVersion();
}

void initGL()
{

	// Set background color
	glClearColor(0.0f, 0.8f, 1.0f, 1.0f);
	// Enable z-buffer test
	glEnable(GL_DEPTH_TEST);

	//constructs all parameters necessary for the loaded shape values
	for(int shape = 0; shape < shapeList.size(); shape++)
		shapeList[shape].init();

	setPid(pid1, "lab7_frag.glsl");

	h_vertPos = GLSL::getAttribLocation(pid1, "vertPos");
	h_vertNor = GLSL::getAttribLocation(pid1, "vertNor");
	h_P = GLSL::getUniformLocation(pid1, "P");
	h_MV = GLSL::getUniformLocation(pid1, "MV");

	h_lightPos1 = GLSL::getUniformLocation(pid1, "lightPos1");
	h_lightPos2 = GLSL::getUniformLocation(pid1, "lightPos2");
	h_lightIntensity1 = GLSL::getUniformLocation(pid1, "intensity1");
	h_lightIntensity2 = GLSL::getUniformLocation(pid1, "intensity2");
}

void reshapeGL(int w, int h)
{
	// Set view size
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	// Set camera aspect ratio
	camera.setWindowSize(w, h);
}

/*generic methods for circular array implementation.
A call to next at the end will send iter to the beginning and vice-versa*/
template <class T>
int previous(std::vector<T> array, int iter)
{
	if(!iter)
		return array.size() - 1;
	else
		return iter-1;
}

template <class T>
int next(std::vector<T> array, int iter)
{
	if(iter + 1 == array.size())
		return 0;
	else
		return iter + 1;
}

void drawGL()
{
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Create matrix stacks
	MatrixStack P, MV;
	// Apply camera transforms
	P.pushMatrix();
	camera.applyProjectionMatrix(&P);
	MV.pushMatrix();
	camera.applyCameraMatrix(&MV);
	
	// Bind the program
	glUseProgram(pid1);

	glUniformMatrix4fv(h_P, 1, GL_FALSE, glm::value_ptr( P.topMatrix()));
	glUniform3fv(h_lightPos1, 1, glm::value_ptr(lights[0].position));
	glUniform3fv(h_lightPos2, 1, glm::value_ptr(lights[1].position));
	glUniform1f(h_lightIntensity1, lights[0].intensity);
	glUniform1f(h_lightIntensity2, lights[1].intensity);

	/* Draw shapes. necessary trnasformations and materials are stored in 
		each ShapeManager*/
	if(abs(rotate - initialRotate) > maxChange)
	{
		windIncr*=-1;
	}
	rotate+=windIncr;
	shapes[0].draw(MV, h_MV, h_vertPos, h_vertNor);
	//shapes[1].draw(MV, h_MV, h_vertPos, h_vertNor);
	tree->render(MV);
	// for(int i = 0; i < shapes.size(); i++)
	// 	shapes[i].draw(MV, h_MV, h_vertPos, h_vertNor);
	// Unbind the program
	//std::cout << std::endl;
	glUseProgram(0);

	// Pop stacks
	MV.popMatrix();
	P.popMatrix();

	// Double buffer
	glutSwapBuffers();
}

void passiveMotionGL(int x, int y)
{
	mouse.x = x;
	mouse.y = y;
	camera.mouseMoved(mouse);
}

void keyboardGL(unsigned char key, int x, int y)
{
	//cout << key << endl;
	keyDown[key] = true;

	if(keyDown[27])
	{
		// ESCAPE
		exit(0);
	}
	if(keyDown['l'])
	{
		level++;
		tree->changeLevel(level);
	}
	if(keyDown['L'])
	{
		level--;
		tree->changeLevel(level);
	}
	if(keyDown['x'])
	{
		lights[selectedLight].position.x+=0.1;
		rotate++;
		shapes[1].changeRotations(glm::vec3(toRadians(1),0.0f,0.0f));
		//std::cout << std::endl;
	}
	if(keyDown['X'])
	{
		lights[selectedLight].position.x-=0.1;
		rotate--;
		shapes[1].changeRotations(glm::vec3(toRadians(-1),0.0f,0.0f));
		//std::cout << std::endl;
	}
	if(keyDown['y'])
	{
		lights[selectedLight].position.y-=0.1;
		shapes[1].changeRotations(glm::vec3(0.0f,toRadians(-1),0.0f));
		//lprintVec3(shapes[1].rotations, "rotation");
		//std::cout << std::endl;
	}
	if(keyDown['Y'])
	{
		lights[selectedLight].position.y+=0.1;
		shapes[1].changeRotations(glm::vec3(0.0f,toRadians(-1),0.0f));
		//lprintVec3(shapes[1].rotations, "rotation");
		//std::cout << std::endl;
	}
	if(keyDown['z'])
	{
		shapes[1].changeRotations(glm::vec3(0.0f,0.0f,toRadians(-1)));
		//lprintVec3(shapes[1].rotations, "rotation");
		//std::cout << std::endl;
	}
	if(keyDown['Z'])
	{
		shapes[1].changeRotations(glm::vec3(0.0f,0.0f,toRadians(-1)));
		//lprintVec3(shapes[1].rotations, "rotation");
		//std::cout << std::endl;
	}
	if(keyDown['b'] || keyDown['B'])
		camera.toggleFlyMove();
	if(keyDown['f'])
		enable = true;
	else
		enable = false;
	//camera control
	//if(keyDown['Y'])
	
	// Refresh screen
	glutPostRedisplay();
}

void keyboardUpGL(unsigned char key, int x, int y)
{
	keyDown[key] = false;
}
//used to ease current translation factors back to 0
void slowSpeeds()
{
	if(currentX > 0)
			currentX = max(0.0,currentX - SLOWING_SPEED);
		else
			currentX = min(0.0,currentX + SLOWING_SPEED);

		if(currentY > 0)
			currentY = max(0.0,currentY - SLOWING_SPEED);
		else
			currentY = min(0.0,currentY + SLOWING_SPEED);

		if(currentZ > 0)
			currentZ = max(0.0,currentZ - SLOWING_SPEED);
		else
			currentZ = min(0.0,currentZ + SLOWING_SPEED);
		glutPostRedisplay();

}

//checks for WASD input and translates accordingly
void updateSpeeds()
{
	if(keyDown['w'] || keyDown['W'])
	{
		currentZ = avg(currentZ, -SPEED_LIMIT);
	}
	if(keyDown['a'] || keyDown['A'])
	{
		currentX = avg(currentX, -SPEED_LIMIT);
	}
	if(keyDown['s'] || keyDown['S'])
	{
		currentZ = avg(currentZ, SPEED_LIMIT);
	}
	if(keyDown['d'] || keyDown['D'])
	{
		currentX = avg(currentX, SPEED_LIMIT);
	}
}

void idleGL()
{
	int time1 = glutGet(GLUT_ELAPSED_TIME);
	int dt = time1 - time0;
	// Update every 60Hz
	updateSpeeds();
	if(dt > 1000.0/60.0) {
		time0 = time1;
		camera.changeTranslation(glm::vec3(currentX,currentY,currentZ));
		slowSpeeds();
	}
}
/*randomly generate numMaterials of materials to be used in generating 
materials for shapes*/
void makeMaterials(int numMaterials)
{
	glm::vec3 ambient, diffuse, specular;
	float shine;
	//ground, light brown
	ambient = glm::vec3(0.68f, 0.48f, .5f);
	diffuse = glm::vec3(randomShade(), randomShade(), randomShade());
	specular = glm::vec3(randomShade(), randomShade(), randomShade());
	shine = randomShine();

	materials.push_back(Material(ambient,specular,diffuse,shine));
	//nodes/leaves, dark green
	ambient = glm::vec3(0.15f,0.52f,0.02f);
	diffuse = glm::vec3(randomShade(), randomShade(), randomShade());
	specular = glm::vec3(randomShade(), randomShade(), randomShade());
	shine = randomShine();

	materials.push_back(Material(ambient,specular,diffuse,shine));
	//tree color, dark brown
	ambient = glm::vec3(0.35,0.27,0.03);
	diffuse = glm::vec3(randomShade(), randomShade(), randomShade());
	specular = glm::vec3(randomShade(), randomShade(), randomShade());
	shine = randomShine();

	materials.push_back(Material(ambient,specular,diffuse,shine));


	if(numMaterials <= 0)
		numMaterials = 1;
	for(int i = 0; i < numMaterials; i++)
	{
		ambient = glm::vec3(randomShade(), randomShade(), randomShade());
		diffuse = glm::vec3(randomShade(), randomShade(), randomShade());
		specular = glm::vec3(randomShade(), randomShade(), randomShade());
		shine = randomShine();

		materials.push_back(Material(ambient,specular,diffuse,shine));

	}
}

void makeLightObject(Light& light, glm::vec3 position, float intensity)
{
	light.position = position;
	light.intensity = intensity;
}

void makeLights()
{
	Light light1, light2;
	makeLightObject(light1, glm::vec3(1.0f, 1.0f, 1.0f), 0.8);
	makeLightObject(light2, glm::vec3(-1.0f, 1.0f, 1.0f), 0.2);
	lights.push_back(light1);
	lights.push_back(light2);
}

void setupShapes(int numShapes)
{
	//make plane first
	shapes.push_back(ShapeManager(shapeList[1], 
								  materials[0],
									  pid1));
	shapes.back().changeScale(glm::vec3(100.0,0.0001,100.0));
	shapes.back().changeTranslation(glm::vec3(0.0,-1.0,0.0));
	//prism building blocks
	shapes.push_back(ShapeManager(shapeList[1], 
									  materials[2],
									  pid1));
	shapes.back().changeScale(glm::vec3(-0.5,-0.5,1)); //scale = 0.5
	shapes.back().changeTranslation(glm::vec3(0.0f,-1.0f,0.0f));
	shapes.back().changeRotations(glm::vec3(toRadians(90),0.0f,0.0f)); 

	//grass
	shapes.push_back(ShapeManager(shapeList[2], 
									  materials[2],
									  pid1));
	shapes.back().changeScale(glm::vec3(-0.9,-0.9,-0.9)); //scale = 0.5
	shapes.back().changeTranslation(glm::vec3(0.0f,0.0f,0.0f));
	shapes.back().changeRotations(glm::vec3(0.0f,0.0f,0.0f)); 
	//leaves
	shapes.push_back(ShapeManager(shapeList[4], 
									  materials[1],
									  pid1));
	shapes.back().changeScale(glm::vec3(-0.1)); //scale = 0.5
	shapes.back().changeTranslation(glm::vec3(0.0f,0.0f,0.0f));
	shapes.back().changeRotations(glm::vec3(0.0f,0.0f,0.0f)); 
}


void insertShape(std::string file)
{
	shapeList.push_back(Shape());
	loadScene(file, shapeList.size()-1);
}

int main(int argc, char **argv)
{
	
	std::string file;
	std::string currentLSystem;
	int numShapes = 1; //default, can be change by command line parameters
	
	if(argc > 1)
	{
		currentLSystem = argv[1];
	}

	//GLUT setup
	glutInit(&argc, argv);
	
	glutInitWindowSize(400, 400);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("LeJon McGowan");
	glutPassiveMotionFunc(passiveMotionGL);
	glutKeyboardFunc(keyboardGL);
	glutKeyboardUpFunc(keyboardUpGL);
	glutReshapeFunc(reshapeGL);
	glutDisplayFunc(drawGL);
	glutIdleFunc(idleGL);
	
	glutFullScreen();
	//Class structure setup
	makeMaterials(11);
	makeLights();
	
	insertShape("bunny.obj");
	insertShape("cube.obj");
	insertShape("Grass_01.obj");
	insertShape("sphere.obj");
	insertShape("plants2.obj");
	
	initGL();
	
	setupShapes(numShapes);
	
	tree = new LSystem("F",shapes[1]);
	tree->addRule(Rule('F', "F[^-F][+<F][+<F][&+F]F[->F][->F][&F]",forward));
	tree->addRule(Rule('+',"+",turnRight));
	tree->addRule(Rule('-',"-",turnLeft));
	tree->addRule(Rule('&',"&",pitchUp));
	tree->addRule(Rule('^',"^",pitchDown));
	tree->addRule(Rule('>',">",rollLeft));
	tree->addRule(Rule('<',"<",rollRight));
	tree->addRule(Rule('|',"|",reverse));
	tree->addRule(Rule('[',"[",push));
	tree->addRule(Rule(']',"]",pop));
	//"F[&+F]F[->F][->F][&F] [-&<F][<++&F]||F[--&>F][+&F]"
	tree->changeLevel(level);
	
	glutMainLoop();
	return 0;
}
