CXX=g++
CXXFLAGS=-g -Wall

neat: main.cpp objects.h
	$(CXX) main.cpp objects.h -o neat
