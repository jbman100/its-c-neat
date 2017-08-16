#include <iostream>
#include <math.h>
#include <vector>

int Hnum, nid, gid, genid;			    // Historical number initialization, with the same for nodes
int struct_prob = 25, weight_prob = 50;		    // Pobabilities of mutation
bool verbose = false;				    // Verbosity level of the algorithm

int input_nb = 2, output_nb = 1;

///////////////////////////////////////////////////////////////


class node {					    // Defines nodes adapted to the issue at hand
    public:
	int const number;		    // This number is just an identifier, having no other purpose
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
	int const historical_num = Hnum;	    // This is the historical number used to speciate genomes
	int origin,dest;
	float weight;				    // The connection's weight
	bool enabled;
	dendrite(int o, int d, float w, bool e) : origin(o), dest(d), weight(w), enabled(e) {++Hnum;};
};


///////////////////////////////////////////////////////////////


class genome {					    // A genome is a collenction of nodes and dendrites
    public:
	int const ID = gid, generation = genid;
	int specie;
	float fitness;
	std::vector<node*> cells;		    // First, the nodes
	std::vector<dendrite> dendrites;	    // Then the dendrites, used as building instructions
	int first_output = input_nb ,first_hidden = input_nb + output_nb;   // Delimiters to categorise the nodes (input/output/hidden)

	int max_dendrites() {
	    int hidden = cells.size() - first_hidden, malus = 0;
	    if (hidden > 0) { ++malus; };
	    return (input_nb*output_nb + hidden*(input_nb + output_nb + 1) - malus);
	};
	
	genome(std::vector<node*> c, std::vector<dendrite> den) : cells(c), dendrites(den) {++gid;};
};


///////////////////////////////////////////////////////////////


std::pair<int,int> random_pair(int input_nb, int output_nb, int n) {
    std::pair<int,int> choice;
    int a = rand() %(n - output_nb + 1), b = rand() %(n - input_nb + 1);
    if (a >= input_nb and a < input_nb + output_nb) { a = a + output_nb; };
    if (b < input_nb) { b = b + input_nb; };
    std::cout << "Dendrite going from cell " << a << " to cell " << b << " selected.\n";
    return std::make_pair(a,b);
}

void mutate_add_connection(genome* specimen) {
    std::cout << "Adding new dendrite to genome ID n:" << specimen->ID << " ...\n";
    if ((int)specimen->dendrites.size() >= specimen->max_dendrites()) {
	std::cout << "No more dendrites can be added.\n";
    } else {
	int n = specimen->dendrites.size();
	std::vector<std::pair<int,int>> existing;
	existing.push_back(std::pair<int,int> (0,0));
	for (int i = 0; i < n; ++i) {
	    std::pair<int,int> connection;
	    connection.first = specimen->dendrites[i].origin;
	    connection.second = specimen->dendrites[i].dest;
	    existing.push_back(connection);
	};
	std::pair<int,int> potent = random_pair(input_nb,output_nb,specimen->cells.size());
	bool present = true;
	while (present) {
	    present = false;
	    for (int i = 0; i < (int)existing.size(); ++i) {
		std::pair<int,int> tnetop (potent.second,potent.first);
		if (existing[i] == potent or existing[i] == tnetop or potent.first == potent.second) {
		    present = true;
		    std::cout << "Connection already exists.\n";
		    potent = random_pair(input_nb,output_nb,specimen->cells.size());
		};
	    };
	};
	float weight = 20 - (float)(rand() %200)/5;
	specimen->dendrites.push_back(dendrite (potent.first,potent.second,weight,true));
	std::cout << "Genome has mutated in structure.\n";
    };
}



void build(genome* specimen) {			    // Build the network according to the dendrites information
    int n = specimen->dendrites.size();
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


void mutate_weight(genome* specimen) {			    // Unfinished
    int instance;
    int index_conn = specimen->dendrites.size();
    for (int i = specimen->first_output; i < specimen->first_hidden; ++i) {
	instance = rand() %100;
	if (instance < weight_prob) {
	    specimen->cells[i]->bias = 20 - (float)(rand() %200)/5;
	};
    };
    for (int i = specimen->first_hidden; i < (int)specimen->cells.size(); ++i) {
	instance = rand() %100;
	if (instance < weight_prob) {
	    specimen->cells[i]->bias = 20 - (float)(rand() %200)/5;
	};
    };
    for (int i = 0; i < index_conn; ++i) {
	instance = rand() %100;
	if (instance < weight_prob) {
	    specimen->dendrites[i].weight = 20 - (float)(rand() %200)/5;
	};
    };
};


std::vector<std::vector<int>> test_input(genome* specimen, std::vector<std::vector<int>> IN) {
    std::vector<std::vector<int>> OU;
    for (int k = 0; k < (int)IN.size(); ++k) {
	std::vector<int> tmp;
	for (int i = 0; i < specimen->first_output; ++i) {	    // This supposes that the list is equal in length
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


std::vector<node*> copy_cells(std::vector<node*> c1, std::vector<node*> c2) {
    std::cout << "Cell copy initiated.\n";
    std::vector<node*> out;
    int n1 = c1.size(), n2 = c2.size(), p1;
    for (int i = 0; i < n1; ++i) {
	out.push_back(new node (c1[i]->number, c1[i]->bias, c1[i]->type));
    };
    std::cout << "Cells from first genome copied.\n";
    for (int i = 0; i < n2; ++i) {
	bool inserted = false;
	p1 = 0;
	n1 = out.size();
	while (not inserted) {
	    if (out[p1]->number == c2[i]->number) { inserted = true; };
	    if (out[p1]->number > c2[i]->number) { 
		auto it = out.begin();
		out.insert(it + p1 -1,c2[i]);
		inserted = true;
	    };
	    if (p1 == n1) {
		out.push_back(c2[i]);
		inserted = true;
	    };
	};
    };
    return out;
}


std::vector<dendrite> cross(std::vector<dendrite> d1, std::vector<dendrite> d2){
    std::vector<dendrite> d3;
    int n1 = d1.size(), n2 = d2.size();
    int p1 = 0, p2 =0;
    while (p1 < n1 and p2 < n2) {
	if (d1[p1].historical_num == d2[p2].historical_num) {
	    int r = std::rand();
	    if (r%2 == 1) {
		d3.push_back(d1[p1]);
	    } else {
		d3.push_back(d2[p2]);
	    };
	    ++p1;
	    ++p2;
	};
	if (d1[p1].historical_num > d2[p2].historical_num) {
	    d3.push_back(d2[p2]);
	    ++p2;
	};
	if (d1[p1].historical_num < d2[p2].historical_num) {
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
    return d3;
};


genome* breed(genome *g1, genome *g2) {
    if (g1->fitness < g2->fitness) {
	return breed(g2,g1);
    };
    std::vector<node*> g3_nod {};
    std::vector<dendrite> g3_den {};
    g3_nod = copy_cells(g1->cells,g2->cells);
    g3_den = cross(g1->dendrites,g2->dendrites);
    return new genome (g3_nod,g3_den);
};
