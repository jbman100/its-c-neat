CXX=g++
CXXFLAGS=-g -Wall -std=c++0x

neat: main.o mutation.o environment.h objects.o tools.h
	$(CXX) $(CXXFLAGS) objects.o mutation.o main.o -o neat

mutation.o: mutation.cpp mutation.h objects.h
	$(CXX) $(CXXFLAGS) -c mutation.cpp -o mutation.o

main.o: main.cpp environment.h tools.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

objects.o: objects.cpp objects.h
	$(CXX) $(CXXFLAGS) -c objects.cpp -o objects.o

clean:
	rm objects.o main.o mutation.o
