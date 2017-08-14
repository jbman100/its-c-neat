#include <iostream>
#include "tools.h"

int main() {					    // Defines a network acting as a XOR gate for experimental purposes
    node* in1 = new node (0,'i');
    node* in2 = new node (0,'i');
    node* ou = new node (6,'o');
    node* hi = new node (3,'h');

    std::vector<node*> nod = {in1,in2,ou,hi};	    // Defines nodes

    dendrite c1 (0,in1,hi,-7.0,true);
    dendrite c2 (1,in2,hi,-13.0,true);
    dendrite c3 (2,in1,ou,-4,true);
    dendrite c4 (3,in2,ou,-4.0,true);
    dendrite c5 (4,hi,ou,-10.0,true);

    std::vector<dendrite> den = {c1,c2,c3,c4,c5};   // Defines dendrites

    genome test (nod,den,2,3);			    // Pipe it into a genome
    test.build();				    // And build the corresponding network

    std::vector<std::vector<int>> vec;
    int query;
    for (int j = 0; j < 4; ++j) {
	std::vector<int> tmp;
	for (int i = 0; i < 2; ++i) {		    // Query user for values
	    std::cout << "Input " << i+1 << "'s value: ";
	    std::cin >> query;
	    tmp.push_back(query);
	};
	vec.push_back(tmp);
    };
    std::cout << "\n";

    
    reckon(vec,test.test_input(vec));		    // Output the result
    //stat_gen(test);
    std::cout << std::endl;

    return 0;
};
