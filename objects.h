#include <iostream>
#include <math.h>
#include <vector>

int Hnum, Nnum, Gnum, gen;			    // Historical number initialization, with the same for nodes
int struct_prob = 25, weight_prob = 50;		    // Pobabilities of mutation
bool verbose = false;				    // Verbosity level of the algorithm


///////////////////////////////////////////////////////////////


class node {					    // Defines nodes adapted to the issue at hand
    public:
	int const number = Nnum;		    // This number is just an identifier, having no other purpose
	int value;				    // Value is relative to the input and will be calculated later
	bool evaluated = false, input, output;	    // This bool prevents calculating a nod's value multiple times
	float bias;				    // The node's bias
	node(float b, char c) : bias (b) {
	    ++Nnum;
	    if (c == 'i') {
		input = true;
	    }
	    if (c == 'o') {
		output = true;
	    };
	};

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
	if (verbose) {
	    std::cout << "Node n:" << node::number << " called.\n";
	};
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


///////////////////////////////////////////////////////////////


class dendrite {				    // A dendrite is a connection between nodes
    public:
	int num;				    // This is the historical number used to speciate genomes
	node* origin;				    // Points to the outputting node
	node* dest;				    // Points to the receiving node
	float weight;				    // The connection's weight
	bool enabled;
	dendrite(int n, node* o, node* d, float w, bool e) : num(n), origin(o), dest(d), weight(w), enabled(e) {};
};


///////////////////////////////////////////////////////////////


class genome {					    // A genome is a collenction of nodes and dendrites
    public:
	int const ID = Gnum, generation = gen;
	int specie;
	float fitness;
	std::vector<node*> cells;		    // First, the nodes
	std::vector<dendrite> dendrites;	    // Then the dendrites, also used as building instructions
	int first_output,first_hidden;		    // Delimiters to categorise the nodes (input/output/hidden)
	genome(std::vector<node*> c, std::vector<dendrite> den, int fo, int fh) : cells(c), dendrites(den), first_output(fo), first_hidden(fh) {++Gnum;};
	
	void build();				    // Build the network according to the dendrites information
	void mutate_weight();			    // Mutate weight inside the genome
	void mutate_struct();			    // Mutate the structure of the genome

	std::vector<std::vector<int>> test_input(std::vector<std::vector<int>>);	// Test an input
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

std::vector<std::vector<int>> genome::test_input(std::vector<std::vector<int>> IN) {
    std::vector<std::vector<int>> OU;
    for (int k = 0; k < (int)IN.size(); ++k) {
	std::vector<int> tmp;
	for (int i = 0; i < first_output; ++i) {	    // This supposes that the list is equal in length
	    (*genome::cells[i]).value = IN[k][i];	    // to the number of inputs, and assign them values
	    (*genome::cells[i]).evaluated = true;	    // Flag to avoid calculating an inputs value
	};
	for (int i = first_output; i < first_hidden; ++i) {
	    tmp.push_back((*genome::cells[i]).get_value(verbose));
	};
	OU.push_back(tmp);
	for (int i = 0; i < (int)genome::cells.size(); ++i) {
	genome::cells[i]->evaluated = false;
	};
    };
    return OU;
};


///////////////////////////////////////////////////////////////


float calc_fit(std::vector<std::vector<int>> ref, std::vector<std::vector<int>> result) {
    int n = ref.size(), out = ref[0].size();
    float fitness = 4;
    for (int i = 0; i < n; ++i) {
	int sum = 0;
	for (int j = 0; j < out; ++j) {
	    sum = sum + ref[i][j] - result[i][j];
	};
	fitness = fitness - std::sqrt(pow(sum,2));
    };
    return fitness;
}
