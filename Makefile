CXX=g++
CXXFLAGS=-g -Wall -std=c++0x

neat: main.o mutation.o objects.o breeding.o evolution.o tools.h
	$(CXX) $(CXXFLAGS) tools.h objects.o mutation.o breeding.o evolution.o main.o -o neat

mutation.o: mutation.cpp mutation.h objects.h
	$(CXX) $(CXXFLAGS) -c mutation.cpp -o mutation.o

main.o: main.cpp evolution.h tools.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

objects.o: objects.cpp objects.h
	$(CXX) $(CXXFLAGS) -c objects.cpp -o objects.o

breeding.o: breeding.cpp breeding.h objects.h
	$(CXX) $(CXXFLAGS) -c breeding.cpp -o breeding.o

evolution.o: evolution.cpp evolution.h mutation.h objects.h
	$(CXX) $(CXXFLAGS) -c evolution.cpp -o evolution.o

###############################################################

clean: objects.o main.o mutation.o breeding.o evolution.o
	rm objects.o main.o mutation.o breeding.o evolution.o
