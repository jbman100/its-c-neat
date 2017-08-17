#include <iostream>
#include <math.h>
#include <vector>



int Hnum = 0, nid = 0, gid = 0, genid = 0, sid = 0; // Historical number initialization, with the same for nodes
int struct_prob = 25, weight_prob = 50;		    // Pobabilities of mutation
bool verbose = false;				    // Verbosity level of the algorithm

float exw = 1, diw = 1, avw = 1, delta = 1;

int input_nb = 2, output_nb = 1;



///////////////////////////////////////////////////////////////
/////////////////////////////// Node class ////////////////////
///////////////////////////////////////////////////////////////



class node {					    // Defines nodes adapted to the issue at hand
    public:
	int const number;			    // This number is just an identifier, having no other purpose
	int value;				    // Value is relative to the input and will be calculated later
	bool evaluated = false;			    // This bool prevents calculating a nod's value multiple times
	float bias;				    // The node's bias
	char type;
	node(int n, float b, char c) : number(n), bias (b), type(c){};

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



///////////////////////////////////////////////////////////////
/////////////////////////////// Dendrite class ////////////////
///////////////////////////////////////////////////////////////



class dendrite {				    // A dendrite is a connection between nodes
    public:
	int const historical_num = Hnum;	    // This is the historical number used to speciate genomes
	int origin,dest;
	float weight;				    // The connection's weight
	bool enabled;
	dendrite(int o, int d, float w, bool e) : origin(o), dest(d), weight(w), enabled(e) {++Hnum;};
};



///////////////////////////////////////////////////////////////
/////////////////////////////// Genome class //////////////////
///////////////////////////////////////////////////////////////



class genome {					    // A genome is a collenction of nodes and dendrites
    public:
	int const ID = gid, generation = genid;
	int species;
	float fitness;
	std::vector<node*> cells;		    // First, the nodes
	std::vector<dendrite> dendrites;	    // Then the dendrites, used as building instructions
	int first_output = input_nb ,first_hidden = input_nb + output_nb;   // Delimiters

	genome(std::vector<node*> c, std::vector<dendrite> den) : cells(c), dendrites(den) {++gid;};
};



///////////////////////////////////////////////////////////////
/////////////////////////////// Genome methods ////////////////
///////////////////////////////////////////////////////////////



int enabled_dendrites(genome* specimen) {
    int c = 0, n = specimen->dendrites.size();
    for (int i = 0; i < n; ++i) {
	if (specimen->dendrites[i].enabled) {
	    ++c;
	}
    }
    return c;
}


int max_dendrites(genome* specimen) {
	    int hidden = specimen->cells.size() - specimen->first_hidden, malus = 0;
	    if (hidden > 0) { ++malus; };
	    return (input_nb*output_nb + hidden*(input_nb + output_nb + 1) - malus);
};


void build(genome* specimen) {			    // Build the network according to the dendrites information

    int n = specimen->cells.size();
    for (int i = 0; i < n; ++i) {		    // Erase previous data
	specimen->cells[i]->peers = {};
    };

    n = specimen->dendrites.size();
    for (int i = 0; i < n; ++i) {
	if (specimen->dendrites[i].enabled) {	    // If the dendrite is enabled, create a pair, traducing it,
	    node* origin;
	    node* dest;
	    int C = specimen->cells.size();
	    for (int j = 0; j < C; ++j) {
		if (specimen->cells[j]->number == specimen->dendrites[i].origin) { origin = specimen->cells[j]; };
		if (specimen->cells[j]->number == specimen->dendrites[i].dest) { dest = specimen->cells[j]; };
	    };
	    std::pair<node*,float> tmp (origin,specimen->dendrites[i].weight);
	    dest->peers.push_back(tmp);
	};
    };
};


void kill(genome* specimen) {

    int n = specimen->cells.size();
    for (int i = n-1; i >= 0; --i) {
	delete specimen->cells[i];
    }
    delete specimen;
}


std::vector<std::vector<int>> test_input(genome* specimen, std::vector<std::vector<int>> IN) {
    std::vector<std::vector<int>> OU;
    for (int k = 0; k < (int)IN.size(); ++k) {
	std::vector<int> tmp;
	for (int i = 0; i < specimen->first_output; ++i) {  // This supposes that the list is equal in length
	    specimen->cells[i]->value = IN[k][i];	    // to the number of inputs, and assign them values
	    specimen->cells[i]->evaluated = true;	    // Flag to avoid calculating an inputs value
	};
	for (int i = specimen->first_output; i < specimen->first_hidden; ++i) {
	    tmp.push_back(specimen->cells[i]->get_value(verbose));
	};
	OU.push_back(tmp);
	for (int i = 0; i < (int)specimen->cells.size(); ++i) {
	specimen->cells[i]->evaluated = false;
	};
    };
    return OU;
};


float calc_fit(genome* specimen, std::vector<std::vector<int>> input, std::vector<std::vector<int>> ref) {
    std::vector<std::vector<int>> result = test_input(specimen, input);
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
};



///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Species class ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



class species {
    public:
	int const ID = sid;
	std::vector<genome*> members = {};
	genome* alpha_g;
	species(genome* g) : alpha_g(g) { ++sid; members.push_back(alpha_g); }
};
