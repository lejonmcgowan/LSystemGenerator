#include "lsystem.h"

using namespace std;

void log(std::string file, int line) {
	std::cout << file << ":" << line << "\n";
}

#define HERE() log(__FILE__, __LINE__)

void LSystem::pushState()
{
	state.push(shape);
}

void LSystem::popState()
{
	shape = state.top();
	state.pop();
}

void LSystem::changeLevel(int newLevel)
{
	if(newLevel < level)
	{
		pattern = axiom;
		nextLevel(newLevel);
	}
	else
	{
		nextLevel(newLevel - level);
	}
	level = newLevel;
}

void LSystem::nextLevel(int numTimes)
{
	for(int i = 0; i < numTimes; i++)
	{
		std::string newPattern = "";
		for(auto letter: pattern)
		{
			newPattern+=findRule(letter).out;
		}
		pattern = newPattern;
	}

}

Rule& LSystem::findRule(char key)
{
	for(int i = 0; i < rules.size(); i++)
		{
			if(key == rules[i].in)
			{
				//cout << "rule found, performing action" << endl;
				return rules[i];
			}
		}
}

void LSystem::addRule(Rule rule)
{
	rules.push_back(rule);
}

void LSystem::render(MatrixStack& MV)
{
	//cout << "rendering pattern" << " of size " << pattern.size() << endl;
	shape.resetAll();
	for(auto letter: pattern)
	{
		//cout << "finding rule for " << letter << endl;
		findRule(letter).action(shape, MV);
	}
}
/*void testSuite()
{
	LSystem *test;
	test = new LSystem("FX");
	test->addRule(Rule('X',"X+YF",nothing));
	test->addRule(Rule('Y',"FX-Y",nothing));
	test->addRule(Rule('F',"F",nothing));
	test->addRule(Rule('+',"+",nothing));
	test->addRule(Rule('-',"-",nothing));
	test->changeLevel(1);
}
int main()
{
	testSuite();
}*/
