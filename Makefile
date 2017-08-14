CXX=g++
CXXFLAGS=-g -Wall

neat: main.cpp objects.h tools.h
	$(CXX) main.cpp objects.h tools.h -o neat
