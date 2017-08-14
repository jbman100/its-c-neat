#include <iostream>
#include "objects.h"

void stat_den(dendrite obsc) {			    // Status of a dendrite
    if (obsc.enabled) {
	std::cout << "Dendrite number " << obsc.num << " goes from cell " << (*obsc.origin).number << " to cell " << (*obsc.dest).number << ", has weight " << obsc.weight << " and is enabled.\n";
    } else {
	std::cout << "Dendrite number " << obsc.num << " is disabled.\n";
    };
};

void stat_node(node* obsc) {			    // Status of a node
    int p = obsc->peers.size();
    std::cout << "Cell number " << obsc->number << " has " << p << " peers and is biased by " << obsc->bias << ".\n";
    for (int i = 0; i < p; ++i) {
	std::cout << "It is linked to node " << obsc->peers[i].first->number << " with weight " << obsc->peers[i].second << ".\n";
    };
};

void stat_gen(genome obsc) {			    // Status of a genome
    int c = obsc.cells.size(), d = obsc.dendrites.size();
    std::cout << "Genome has " << c << " cells.\n";
    for (int i = 0; i < c; ++i) {
	stat_node(obsc.cells[i]);
    };
    std::cout << "Genome has " << d << " dendrites.\n";
    for (int i = 0; i < d; ++i) {
	stat_den(obsc.dendrites[i]);
    };
};


int main() {					    // Defines a network acting as a XOR gate for experimental purposes
    node* in1 = new node (0);
    node* in2 = new node (0);
    node* ou = new node (6);
    node* hi = new node (3);

    std::vector<node*> nod = {in1,in2,ou,hi};	    // Defines nodes

    dendrite c1 (0,in1,hi,-7.0,true);
    dendrite c2 (1,in2,hi,-13.0,true);
    dendrite c3 (2,in1,ou,-4.0,true);
    dendrite c4 (3,in2,ou,-4.0,true);
    dendrite c5 (4,hi,ou,-10.0,true);

    std::vector<dendrite> den = {c1,c2,c3,c4,c5};   // Defines dendrites

    genome test (nod,den);			    // Pipe it into a genome
    test.build();				    // And build the corresponding network

    std::vector<int> vec;
    int query;
    for (int i = 0; i < 2; ++i) {		    // Query user for values
	std::cout << "Input " << i+1 << "'s value: ";
	std::cin >> query;
	vec.push_back(query);
    };	

    std::cout << "The result is: " << test.test_input(vec)[0] << "\n";	// Output the result

    return 0;
};
