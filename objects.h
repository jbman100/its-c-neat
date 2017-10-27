//         _     _           _         _    
//    ___ | |__ (_) ___  ___| |_ ___  | |__  
//   / _ \| '_ \| |/ _ \/ __| __/ __| | '_ | 
//  | (_) | |_) | |  __/ (__| |_\__ \_| | | |
//   \___/|_.__// |\___|\___|\__|___(_)_| |_|
//            |__/                           

#ifndef OBJECTS_H
#define OBJECTS_H

#include <iostream>
#include <cmath>
#include <vector>


extern int Hnum, nid, gid, genid, sid; // Historical number initialization, with the same for nodes
extern int addNodeProb, addDendriteProb, changeWeightProb;		    // Pobabilities of mutation
extern bool verbose;				    // Verbosity level of the algorithm

extern int input_nb, output_nb;

extern float* population_average;

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Node class ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



class node {					    // Defines nodes adapted to the issue at hand
    public:
	int const number;			    // This number is just an identifier, having no other purpose
	int value;				    // Value is relative to the input and will be calculated later
	bool evaluated = false;			    // This bool prevents calculating a nod's value multiple times
	float bias;				    // The node's bias
	char const type;
	node(int n, float b, char c) : number(n), bias (b), type(c){};
	node(int n) : number(n), value(0), type('h') {};

	std::vector<std::pair<node*,float>> peers;  // This vector holds information about the NE structure
	float transfer(float);			    // The transfer function on the node
	float get_value(bool);			    // The function outputting the nodes value
};



///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Dendrite class ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



class dendrite {				    // A dendrite is a connection between nodes
    public:
	int const historical_num = Hnum, origin, dest;
	float weight;				    // The connection's weight
	bool enabled = true;
	dendrite(int o, int d, float w) : origin(o), dest(d), weight(w) {++Hnum;};
	dendrite(int h, int o, int d, float w) : historical_num(h), origin(o), dest(d), weight(w) {};
};



///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Genome class //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



class genome {					    // A genome is a collenction of nodes and dendrites
    public:
	std::vector<node*> cells;		    // First, the nodes
	std::vector<dendrite> dendrites;	    // Then the dendrites, used as building instructions
	int const ID = gid, generation = genid, first_output = input_nb, first_hidden = input_nb + output_nb;
	float fitness = 0;
	int species = 0;

	genome(std::vector<node*> c, std::vector<dendrite> den) : cells(c), dendrites(den) {++gid;};
};



///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Species class ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



class species {
    public:
	int const ID = sid;
	int allowed_offspring = 2;
	float average_fitness = 0;
	std::vector<genome*> members = {};
	genome* alpha_g;

	void calc_average();

	species(genome* g) : alpha_g(g) { ++sid; members.push_back(alpha_g); }
};



///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Genome methods ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


typedef std::vector<species*>* pop;

int enabled_dendrites(genome*);

int max_dendrites(genome*);

void build(genome*);

void kill(genome*);

std::vector<std::vector<int>> test_input(genome*, std::vector<std::vector<int>>);

float calc_fit(genome*, std::vector<std::vector<int>>, std::vector<std::vector<int>>, std::vector<species*>*);

#endif
