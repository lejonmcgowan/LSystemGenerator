all:
	g++ -std=c++11 -Wno-deprecated *.cpp *.cc -DGL_GLEXT_PROTOTYPES -lglut -lGL -lGLU -O3 -o final
debug:
	g++ -g -std=c++11 -Wno-deprecated *.cpp *.cc -DGL_GLEXT_PROTOTYPES -lglut -lGL -lGLU -o final

osx:
	g++ -std=c++11 -Wno-deprecated *.cpp *.cc -framework GLUT -framework OpenGL

clean:
	rm -f *~ *.o final
