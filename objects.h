#include <iostream>
#include <math.h>
#include <vector>

int Hnum = 0, Nnum = 0;
int struct_prob = 25, weight_prob = 50;
class node;


class dendrite {
    public:
	int num;
	node* origin;
	node* dest;
	float weight;
	bool enabled;
	dendrite(int n, node* o, node* d, float w, bool e) : num(n), origin(o), dest(d), weight(w), enabled(e) {};
};


class node {
    public:
	int number;
	std::vector<std::pair<node*,float>> peers;
	float bias;
	float transfer(float input) {
	    return (1/(1+std::exp(input*-4)));
	};
	node(float b) : bias (b) {number = Nnum; ++Nnum;};
};


class genome {
    public:
	std::vector<node> cells;
	int first_output,first_hidden;
	std::vector<dendrite> dendrites;
	genome(std::vector<node> c, std::vector<dendrite> den) : cells(c), dendrites(den) {};
	void mutate_weight();
	void mutate_struct();
	void build();
};


void genome::mutate_weight() {
    int instance;
    int index_conn = dendrites.size();
    for (int i = first_output; i < first_hidden; ++i) {
	instance = rand() %100;
	if (instance < weight_prob) {
	    cells[i].bias = (float)(rand() %100)/100;
	};
    };
    for (int i = first_hidden; i < (int)cells.size(); ++i) {
	instance = rand() %100;
	if (instance < weight_prob) {
	    cells[i].bias = (float)(rand() %100)/100;
	};
    };
    for (int i = 0; i < index_conn; ++i) {
	instance = rand() %100;
	if (instance < weight_prob) {
	    dendrites[i].weight = (float)(rand() %100)/100;
	};
    };
};

void genome::mutate_struct() {
    int instance = rand() %100;
    if (instance < struct_prob) {
	node tmp ((float)(rand() %100)/100);
	cells.push_back(tmp);
    };
};

void genome::build() {
    int n = dendrites.size();
    for (int i = 0; i < n; ++i) {
	std::pair<node*,float> tmp (dendrites[i].origin,dendrites[i].weight);
	std::cout << "Adding node " << (*dendrites[i].origin).number << " to list of node " << (*dendrites[i].dest).number << "'s peers.\n";
	(*dendrites[i].dest).peers.push_back(tmp);
    };
}
