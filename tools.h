#include <string>
#include <iostream>
#include "objects.h"

void stat_den(dendrite obsc) {			    // Status of a dendrite
    if (obsc.enabled) {
	std::cout << "Dendrite number " << obsc.num 
	    << " goes from cell " << obsc.origin
	    << " to cell " << obsc.dest
	    << ", has weight " << obsc.weight 
	    << " and is enabled.\n";
    } else {
	std::cout << "Dendrite number " 
	    << obsc.num << " is disabled.\n";
    };
};

void stat_node(node* obsc) {			    // Status of a node
    std::cout << obsc->number << ": cell" ;
    int p = obsc->peers.size();
    if ((*obsc).type == 'i') {
	std::cout << " is an input.\n";
    } else {
	if ((*obsc).type == 'o') {
	    std::cout << " is an output,";
	};
	std::cout << " has " << p << " peers and is biased by " << obsc->bias << ".\n";
	for (int i = 0; i < p; ++i) {
	    std::cout << " -> It is linked to cell " << obsc->peers[i].first->number << " with weight " << obsc->peers[i].second << ".\n";
	};
    };
};

void stat_gen(genome* obsc) {			    // Status of a genome
    std::cout << "\nGetting the status of genome, ID n: " << obsc->ID << "...\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Genome is part of generation n: " << obsc->generation << ".\n";
    std::cout << "Genome is part of specie n: " << obsc->specie << ".\n";
    int c = obsc->cells.size(), d = obsc->dendrites.size();
    std::cout << "Genome has " << c << " cells.\n";
    for (int i = 0; i < c; ++i) {
	stat_node(obsc->cells[i]);
    };
    std::cout << "Genome has " << d << " dendrites.\n";
    for (int i = 0; i < d; ++i) {
	stat_den(obsc->dendrites[i]);
    };
    std::cout << std::endl;
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

std::vector<std::vector<int>> manual_in() {
    int amount, inputs, query;
    std::cout << "Amount of entries: ";
    std::cin >> amount;
    std::cout << "Amount of inputs: ";
    std::cin >> inputs;
    std::vector<std::vector<int>> vec;
    for (int j = 0; j < 4; ++j) {
	std::vector<int> tmp;
	for (int i = 0; i < 2; ++i) {		    // Query user for values
	    std::cout << "Input " << i+1 << "'s value: ";
	    std::cin >> query;
	    tmp.push_back(query);
	};
	vec.push_back(tmp);
    };
    return vec;
};

std::vector<std::string> process_in() {
    std::vector<std::string> processed;
    std::string raw;
    std::getline(std::cin,raw);
    int n = raw.length();
    int po1=0,po2=0;
    for (po2=0; po2 < n; ++po2) {
	if (raw.at(po2) == ' ') {
	    processed.push_back(raw.substr(po1,po2-po1));
	    po1=po2+1;
	};
    };
    processed.push_back(raw.substr(po1));
    return processed;
};


void ls_pop(std::vector<std::vector<genome*>> population) {
    int n = population.size();
    if (n == 0) {
	std::printf("The population is empty. Try creating a genome !\n");
    } else {
	for (int i = 0; i < n; ++i) {
	    std::cout << "___ Specie n: " << i << " ___\n";
	    int spe_n = population[i].size();
	    if (spe_n == 0) {
		std::printf("Specie is empty.\n");
	    } else {
		for (int j = 0; j < spe_n; ++j) {
		std::cout << j << " -> Genome ID n: " << population[i][j]->ID << " (" << *population[i][j]->fitness << ")\n";
		};
	    };
	};
    };
};


void science(std::vector<std::vector<genome*>> population, std::vector<std::vector<int>> input, std::vector<std::vector<int>> reference) {
    int n = population.size(),k;
    for (int i = 0; i < n; ++i) {
	k = population[i].size();
	for (int j = 0; j < k; ++j) {
	    population[i][j]->build();
	    std::cout << "Genome n:" << j << " has been built.\n";
	    float fit = calc_fit(reference,population[i][j]->test_input(input));
	    std::cout << "Genome n:" << j << " in species n:" << i << " has a fitness of " << fit << ".\n";
	    *population[i][j]->fitness = fit;
	};
    };
    std::cout << "science done" << std::endl;
};


void cli(std::vector<std::vector<genome*>> population, std::vector<std::vector<int>> input, std::vector<std::vector<int>> ref) {
    bool finished = false;
    while (!finished) {
	std::vector<std::string> query;
	std::cout << ">> ";
	query = process_in();

	if (query[0] == "q") {
	    finished = true;
	};

	if (query[0] == "ls") {
	    ls_pop(population);
	};

	if (query[0] == "s") {
	    science(population,input,ref);
	};

	if (query[0] == "b") {
	    population[0].push_back(breed(population[0][0],population[0][1]));
	};

	if (query[0] == "status") {
	    if (query.size() >= 3) {
		int sp = std::stoi(query[1]), id = std::stoi(query[2]);
		stat_gen(population[sp][id]);
	    } else {
		std::cout << "Status takes the species and the index of the target genome as arguments." << std::endl;
	    };
	};
    };
};
