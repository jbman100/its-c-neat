CXX=g++
CXXFLAGS=-g -Wall

neat: main.cpp objects.h tools.h
	$(CXX) $(CXXFLAGS) objects.h environment.h tools.h main.cpp -o neat
