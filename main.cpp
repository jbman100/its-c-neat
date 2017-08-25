#include <iostream>
#include "tools.h"

int Hnum = 0, nid = 0, gid = 0, genid = 0, sid = 0; // Historical number initialization, with the same for nodes
int addNodeProb = 25, addDendriteProb = 50, changeWeightProb = 50;                  // Pobabilities of mutation
bool verbose = false;                               // Verbosity level of the algorithm

int input_nb = 2, output_nb = 1;


std::vector<species*>* population = new std::vector<species*>;
std::vector<std::vector<int>> input {{1,0},{0,1},{1,1},{0,0}};
std::vector<std::vector<int>> reference {{1},{1},{0},{0}};

int main() {					    // Defines a network acting as a XOR gate for experimental purposes
    std::srand(time(0));
    verbose = true;
    nid = 4;

    dendrite c1 (0,3,-7.0,true);
    dendrite c2 (1,3,-13.0,true);
    dendrite c3 (0,2,-4,true);
    dendrite c4 (1,2,-4.0,true);
    dendrite c5 (3,2,-10.0,true);

    std::vector<dendrite> den = {c1,c2,c3,c4,c5};   // Defines dendrites

    categorize(new genome({new node(0,0,'i'), new node(1,0,'i'), new node(2,0,'o'), new node(3,0,'h')},{c1,c2,c3,c5}), population);
    categorize(new genome({new node(0,0,'i'), new node(1,0,'i'), new node(2,0,'o'), new node(3,0,'h')},{c1,c2,c4}), population);

    categorize(breed((*population)[0]->members[0],(*population)[0]->members[1]),population);
    categorize(breed((*population)[0]->members[0],(*population)[0]->members[1]),population);
    categorize(breed((*population)[0]->members[0],(*population)[0]->members[1]),population);

    cli(population, input, reference);
    return 0;
};
