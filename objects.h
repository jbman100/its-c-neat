#include <iostream>
#include <math.h>
#include <vector>

int Hnum, Nnum;					    // Historical number initialization, with the same for nodes
int struct_prob = 25, weight_prob = 50;		    // Pobabilities of mutation
bool verbose = false;				    // Verbosity level of the algorithm


class node {					    // Defines nodes adapted to the issue at hand
    public:
	int const number = Nnum;		    // This number is just an identifier, having no other purpose
	int value;				    // Value is relative to the input and will be calculated later
	bool evaluated = false;			    // This bool prevents calculating a nod's value multiple times
	float bias;				    // The node's bias
	node(float b) : bias (b) {++Nnum;};

	std::vector<std::pair<node*,float>> peers;  // This vector holds information about the NE structure
	float transfer(float);			    // The transfer function on the node
	float get_value(bool);			    // The function outputting the nodes value
};

float node::transfer(float input) {
    float value = (1/(1+std::exp(input*-4)));	    // A simple transfer function.
    if (value > 0.9) {
	return 1;
    } else {
	return 0;
    };
};

float node::get_value(bool verbose) {
    if (evaluated) {				    // If previously evaluated: just output the value
	return value;
    } else {
	for (int i = 0; i < (int)peers.size(); ++i) {
	    value = value + (*peers[i].first).get_value(verbose)*(peers[i].second);
	};
	value = value + bias;
	if (verbose) {std::cout << "Node " << number << "'s value is " << value << " so it outputs " << transfer(value) << "\n";};	//Debug clause
	value = transfer(value);
	evaluated = true;
	return value;
    };
};


class dendrite {				    // A dendrite is a connection between nodes
    public:
	int num;				    // This is the historical number used to speciate genomes
	node* origin;				    // Points to the outputting node
	node* dest;				    // Points to the receiving node
	float weight;				    // The connection's weight
	bool enabled;
	dendrite(int n, node* o, node* d, float w, bool e) : num(n), origin(o), dest(d), weight(w), enabled(e) {};
};


class genome {					    // A genome is a collenction of nodes and dendrites
    public:
	std::vector<node*> cells;		    // First, the nodes
	int first_output,first_hidden;		    // Delimiters to categorise the nodes (input/output/hidden)
	std::vector<dendrite> dendrites;	    // Then the dendrites, also used as building instructions
	genome(std::vector<node*> c, std::vector<dendrite> den) : cells(c), dendrites(den) {};
	
	void build();				    // Build the network according to the dendrites information
	void mutate_weight();			    // Mutate weight inside the genome
	void mutate_struct();			    // Mutate the structure of the genome

	std::vector<int> test_input(std::vector<int>);	// Test an input, VERY experimental
};


void genome::build() {
    int n = dendrites.size();
    for (int i = 0; i < n; ++i) {
	if (dendrites[i].enabled) {		    // If the dendrite is enabled, create a pair, traducing it,
	    std::pair<node*,float> tmp (dendrites[i].origin,dendrites[i].weight);   // at the destination node
	    dendrites[i].dest->peers.push_back(tmp);
	};
    };
};


void genome::mutate_weight() {			    // Unfinished
    int instance;
    int index_conn = dendrites.size();
    for (int i = first_output; i < first_hidden; ++i) {
	instance = rand() %100;
	if (instance < weight_prob) {
	    (*cells[i]).bias = (float)(rand() %100)/100;
	};
    };
    for (int i = first_hidden; i < (int)cells.size(); ++i) {
	instance = rand() %100;
	if (instance < weight_prob) {
	    (*cells[i]).bias = (float)(rand() %100)/100;
	};
    };
    for (int i = 0; i < index_conn; ++i) {
	instance = rand() %100;
	if (instance < weight_prob) {
	    dendrites[i].weight = (float)(rand() %100)/100;
	};
    };
};

std::vector<int> genome::test_input(std::vector<int> list) {
    int n = list.size();			    // Supposes that the input list is equal in length
    for (int i = 0; i<n; ++i) {			    // to the number of inputs, and assign them values
	(*genome::cells[i]).value = list[i];
	(*genome::cells[i]).evaluated = true;	    // Flag to avoid calculationg an inputs value
    };
    int N = genome::cells.size();		    // For each remaining cell, recursively calculate an input
    for (int i = n; i<N; ++i) {
	(*genome::cells[i]).get_value(verbose);
    };
    return std::vector<int> {(*genome::cells[2]).value}; // Experimental bit
}
