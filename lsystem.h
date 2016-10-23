#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <functional>

#include "GLSL.h"
#include "ShapeManager.h"
#include "GLSL.h"

class Rule
{
public:	
	char in;
	std::string out;
	void (*action)(ShapeManager&, MatrixStack&);
	Rule(char in, std::string out,
		void (*action)(ShapeManager&, MatrixStack&)): action(action), in(in), out(out){};
};

class LSystem
{
private:
	std::string axiom;
	int level;
	std::stack<ShapeManager> state;
	void nextLevel(int numTimes);
	
	std::vector<Rule> rules;
public:
	ShapeManager shape;

	std::string pattern;
	void pushState();
	void popState();
	void changeLevel(int newLevel);
	void addRule(Rule rule);
	void render(MatrixStack& MV);
	Rule& findRule(char key);

	LSystem(std::string axiom,ShapeManager shape): 
	pattern(axiom), 
	axiom(axiom),
	level(0),
	shape(shape){};
};


