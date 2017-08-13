#include <iostream>
#include "objects.h"

void stat_den(dendrite obsc) {
    if (obsc.enabled) {
	std::cout << "Dendrite number " << obsc.num << " goes from cell " << (*obsc.origin).number << " to cell " << (*obsc.dest).number << ", has weight " << obsc.weight << " and is enabled.\n";
    } else {
	std::cout << "Dendrite number " << obsc.num << " is disabled.\n";
    };
};

void stat_node(node obsc) {
    std::cout << "Cell number " << obsc.number << " has " << obsc.peers.size() << " peers and is biased by " << obsc.bias << ".\n";
};

void stat_gen(genome obsc) {
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


int main() {
    node in1 (0);
    node in2 (0);
    node ou (2);
    node hi (1);

    std::vector<node> nod = {in1,in2,ou,hi};

    dendrite c1 (0,&in1,&hi,-1,true);
    dendrite c2 (1,&in2,&hi,-1,true);
    dendrite c3 (2,&in1,&ou,-1,true);
    dendrite c4 (3,&in2,&ou,-1,true);
    dendrite c5 (4,&hi,&ou,-1,true);

    std::vector<dendrite> den = {c1,c2,c3,c4,c5};

    genome test (nod,den);
    test.build();
    
    stat_gen(test);
    return 0;
};
