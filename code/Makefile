CC=g++

CXXFLAGS= -Wall -std=c++11 -I/usr/local/Cellar/boost/1.60.0_2/include

LDFLAGS= -L/usr/local/Cellar/boost/1.60.0_2/lib


integral: integral.cpp
	g++ $(CXXFLAGS) $(LDFLAGS) integral.cpp -o integral.out
	./integral.out

modeler: modeler.cpp
	g++ $(CXXFLAGS) $(LDFLAGS) modeler.cpp -o modeler.out
	./modeler.out

modeler2d: modeler2d.cpp
	g++ $(CXXFLAGS) $(LDFLAGS) modeler2d.cpp -o modeler2d.out
	./modeler2d.out
