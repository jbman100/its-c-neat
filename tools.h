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
    std::cout << obsc->number << ": cell" ;
    int p = obsc->peers.size();
    if ((*obsc).input) {
	std::cout << " is an input.\n";
    } else {
	if ((*obsc).output) {
	    std::cout << " is an output,";
	};
	std::cout << " has " << p << " peers and is biased by " << obsc->bias << ".\n";
	for (int i = 0; i < p; ++i) {
	    std::cout << " -> It is linked to cell " << obsc->peers[i].first->number << " with weight " << obsc->peers[i].second << ".\n";
	};
    };
};

void stat_gen(genome obsc) {			    // Status of a genome
    std::cout << "\nGetting the status of genome, ID n: " << obsc.ID << "...\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Genome is part of generation n: " << obsc.generation << ".\n";
    std::cout << "Genome is part of specie n: " << obsc.specie << ".\n";
    int c = obsc.cells.size(), d = obsc.dendrites.size();
    std::cout << "Genome has " << c << " cells.\n";
    for (int i = 0; i < c; ++i) {
	stat_node(obsc.cells[i]);
    };
    std::cout << "Genome has " << d << " dendrites.\n";
    for (int i = 0; i < d; ++i) {
	stat_den(obsc.dendrites[i]);
    };
}

int reckon(std::vector<std::vector<int>> IN, std::vector<std::vector<int>> OU) {
    int a = IN.size(), b = OU.size();
    if (a != b) {
	std::cout << "An error has occured.\n";
	return 1;
    };
    for (int i = 0; i < a; ++i) {
	int c = IN[i].size(), d = OU[i].size();
	std::cout << "Input " << i << ": [ ";
	for (int j = 0; j < c; ++j) {
	    std::cout << IN[i][j] << " ";
	};
	std::cout << "] --> [ ";
	for (int j = 0; j < d; ++j) {
	    std::cout << OU[i][j] << " ";
	};
	std::cout << "]\n";
    };
    return 0;
};
