#include <iostream>
#include <math.h>
#include <vector>

int Hnum, Gnum, gen;			    // Historical number initialization, with the same for nodes
int struct_prob = 25, weight_prob = 50;		    // Pobabilities of mutation
bool verbose = false;				    // Verbosity level of the algorithm

int input_nb = 2, output_nb = 1;

///////////////////////////////////////////////////////////////


class node {					    // Defines nodes adapted to the issue at hand
    public:
	int number;		    // This number is just an identifier, having no other purpose
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


class dendrite {				    // A dendrite is a connection between nodes
    public:
	int num;				    // This is the historical number used to speciate genomes
	int origin,dest;
	float weight;				    // The connection's weight
	bool enabled;
	dendrite(int n, int o, int d, float w, bool e) : num(n), origin(o), dest(d), weight(w), enabled(e) {};
};


///////////////////////////////////////////////////////////////


class genome {					    // A genome is a collenction of nodes and dendrites
    public:
	int const ID = Gnum, generation = gen;
	int specie;
	float * fitness = new float;
	std::vector<node*> cells;		    // First, the nodes
	std::vector<dendrite> dendrites;	    // Then the dendrites, also used as building instructions
	int first_output = input_nb ,first_hidden = input_nb + output_nb;		    // Delimiters to categorise the nodes (input/output/hidden)
	genome(std::vector<node*> c, std::vector<dendrite> den) : cells(c), dendrites(den) {++Gnum;};
	
	void build();				    // Build the network according to the dendrites information
	void mutate_weight();			    // Mutate weight inside the genome
	void mutate_struct();			    // Mutate the structure of the genome

	std::vector<std::vector<int>> test_input(std::vector<std::vector<int>>);	// Test an input
};


void genome::build() {
    int n = dendrites.size();
    for (int i = 0; i < n; ++i) {
	if (dendrites[i].enabled) {		    // If the dendrite is enabled, create a pair, traducing it,
	    node* origin;
	    node* dest;
	    int C = genome::cells.size();
	    for (int j = 0; j < C; ++j) {
		if (cells[j]->number == dendrites[i].origin) { origin = cells[j]; };
		if (cells[j]->number == dendrites[i].dest) { dest = cells[j]; };
	    };
	    std::pair<node*,float> tmp (origin,dendrites[i].weight);   // at the destination node
	    dest->peers.push_back(tmp);
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
};


///////////////////////////////////////////////////////////////


genome* genome_factory(std::vector<node*> nod, std::vector<dendrite> den) {
    return new genome (nod,den);
}

std::vector<node*> copy_cells(std::vector<node*> original) {
    std::vector<node*> out;
    int n = original.size();
    for (int i = 0; i < n; ++i) {
	out.push_back(new node (original[i]->number,original[i]->bias,original[i]->type));
    };
    return out;
}


std::vector<dendrite> cross(std::vector<dendrite> d1, std::vector<dendrite> d2){
    std::cout << "Initated dendrite crossing\n";
    std::vector<dendrite> d3;
    int n1 = d1.size(), n2 = d2.size();
    int p1 = 0, p2 =0;
    while (p1 < n1 and p2 < n2) {
	if (d1[p1].num == d2[p2].num) {
	    std::cout << "Case 1\n";
	    int r = std::rand();
	    if (r%2 == 1) {
		d3.push_back(d1[p1]);
	    } else {
		d3.push_back(d2[p2]);
	    };
	    ++p1;
	    ++p2;
	};
	if (d1[p1].num > d2[p2].num) {
	    std::cout << "Case 2\n";
	    d3.push_back(d2[p2]);
	    ++p2;
	};
	if (d1[p1].num < d2[p2].num) {
	    std::cout << "Case 3\n";
	    d3.push_back(d1[p1]);
	    ++p1;
	};
    };
    while (p1<n1) {
	d3.push_back(d1[p1]);
	++p1;
    };
    while (p2<n2) {
	d3.push_back(d2[p2]);
	++p2;
    };

    std::cout << "Crossing done.\n";
    return d3;
};

genome* breed(genome *g1, genome *g2) {
    std::cout << "Initated breeding between genome " << g1->ID
	      << " and genome " << g2->ID << "->\n";
    if (*g1->fitness < *g2->fitness) {
	std::cout << "Swapping genomes ...\n";
	return breed(g2,g1);
    };
    std::vector<node*> g3_nod {};
    std::vector<dendrite> g3_den {};
    std::cout << "Generated vectors\n";
    if (g1->cells.size() > g2->cells.size()) {
	std::cout << "Giving the new genome g" << g2->ID << " cells...\n";
	g3_nod = copy_cells(g2->cells);
    } else {
	std::cout << "Giving the new genome g" << g1->ID << " cells...\n";
	g3_nod = copy_cells(g1->cells);
    };
    std::cout << "Cells attributed. Crossing...\n";
    g3_den = cross(g1->dendrites,g2->dendrites);
    std::cout << "New genome added to species 0.\n";
    return genome_factory(g3_nod,g3_den);
};
