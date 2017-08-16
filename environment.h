#include "objects.h"
#include <iostream>



///////////////////////////////////////////////////////////////
/////////////////////////////// Breeding procedure ////////////
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
	std::cout << "Inserting cell " << c2[i]->number << "...\n";
	bool inserted = false;
	p1 = 0;
	n1 = out.size();
	while (not inserted) {
	    if (out[p1]->number == c2[i]->number) { inserted = true; std::cout << "Cell " << c2[i]->number << " already present.\n";};
	    if (out[p1]->number > c2[i]->number) { 
		auto it = out.begin();
		out.insert(it + p1 -1,c2[i]);
		std::cout << "Inserting cell " << c2[i]->number << " before cell " << out[p1+1]->number << ".\n";
		inserted = true;
	    };
	    if (p1 == n1-1 and not inserted) {
		out.push_back(c2[i]);
		std::cout << "Inserting it at the end.\n";
		inserted = true;
	    };
	    ++p1;
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
    std::vector<node*> g3_nod {};
    std::vector<dendrite> g3_den {};
    g3_nod = copy_cells(g1->cells,g2->cells);
    g3_den = cross(g1->dendrites,g2->dendrites);
    return new genome (g3_nod,g3_den);
};



///////////////////////////////////////////////////////////////
/////////////////////////////// Mutations /////////////////////
///////////////////////////////////////////////////////////////


/////////////////////////////// Weight mutation ///////////////


void mutate_weight(genome* specimen) {
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


/////////////////////////////// Adding a node /////////////////


void mutate_add_node(genome* specimen) {
    std::cout << "Adding new node to genome ID n:" << specimen->ID << " ...\n";
    int n = specimen->dendrites.size();
    std::vector<std::pair<int,int>> existing;
    existing.push_back(std::pair<int,int> (0,0));
    for (int i = 0; i < n; ++i) {
	std::pair<int,int> connection;
	connection.first = specimen->dendrites[i].origin;
	connection.second = specimen->dendrites[i].dest;
	existing.push_back(connection);
    };
    n = existing.size();
    int choice = rand() %(n-1) +1;
    std::pair<int,int> to_replace = existing[choice];
    specimen->dendrites[choice - 1].enabled = false;
    specimen->cells.push_back(new node (nid,0,'h'));
    specimen->dendrites.push_back(dendrite(to_replace.first, nid, 1, true));
    specimen->dendrites.push_back(dendrite(nid, to_replace.second, specimen->dendrites[choice-1].weight, true));
    ++nid;
    std::cout << "Genome gained a node.\n";
}


/////////////////////////////// Adding a dendrite /////////////


std::pair<int,int> random_pair(std::vector<int> first_members, std::vector<int> second_members) {
    std::pair<int,int> choice;
    int a = rand() %(int)first_members.size(), b = rand() %(int)second_members.size();
    while (first_members[a] == second_members[b]) { b = rand() %(int)second_members.size(); };
    std::cout << "Dendrite going from cell " << first_members[a] << " to cell " << second_members[b] << " selected.\n";
    return std::make_pair(first_members[a],second_members[b]);
}


void mutate_add_connection(genome* specimen) {
    std::cout << "Adding new dendrite to genome ID n:" << specimen->ID << " ...\n";
    if ((int)specimen->dendrites.size() >= max_dendrites(specimen)) {
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
	std::vector<int> fm {};
	for (int i = 0; i < specimen->first_output; ++i) { fm.push_back(i); std::cout << i << " is an input\n";};
	std::vector<int> sm {};
	for (int i = specimen->first_output; i < specimen->first_hidden; ++i) { sm.push_back(i); std::cout << i << " is an output\n";};
	for (int i = specimen->first_hidden; i < (int)specimen->cells.size(); ++i) {
	    fm.push_back(specimen->cells[i]->number);
	    sm.push_back(specimen->cells[i]->number);
	};
	std::pair<int,int> potent = random_pair(fm,sm);
	bool present = true;
	while (present) {
	    present = false;
	    for (int i = 0; i < (int)existing.size(); ++i) {
		std::pair<int,int> tnetop (potent.second,potent.first);
		if (existing[i] == potent or existing[i] == tnetop) {
		    present = true;
		    std::cout << "Connection already exists.\n";
		    potent = random_pair(fm,sm);
		};
	    };
	};
	float weight = 20 - (float)(rand() %200)/5;
	specimen->dendrites.push_back(dendrite (potent.first,potent.second,weight,true));
	std::cout << "Genome has mutated in structure.\n";
    };
}
