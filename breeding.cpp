#include "breeding.h"


///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Breeding procedure ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



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
	    if (out[p1]->number == c2[i]->number) { inserted = true; };
	    if (out[p1]->number > c2[i]->number) { 
		auto it = out.begin();
		out.insert(it + p1 -1,new node(c2[i]->number, c2[i]->bias, c2[i]->type));
		inserted = true;
	    };
	    if (p1 == n1-1 and not inserted) {
		out.push_back(new node(c2[i]->number, c2[i]->bias, c2[i]->type));
		inserted = true;
	    };
	    ++p1;
	};
    };
    return out;
}


std::vector<dendrite> cross(std::vector<dendrite> d1, std::vector<dendrite> d2, bool equal_fitness){
    if (equal_fitness) { std::cout << "The genomes have the same fitness !\n";}
    std::vector<dendrite> d3;
    int n1 = d1.size(), n2 = d2.size();
    int p2 = 0;
    for (int i = 0; i < n1; ++i) {
	bool inserted = false;
	while (not inserted) {
	    std::cout << "New iteration, i = " << i << ", p2 = " << p2 << ".\n";
	    if (d1[i].historical_num == d2[p2].historical_num) {
		int choice = rand() %2;
		if (choice == 0) {
		    d3.push_back(d1[i]);
		} else {
		    d3.push_back(d2[p2]);
		}
		++p2;
		inserted = true;
	    }

	    if ((d1[i].historical_num < d2[p2].historical_num or p2 == n2) and not inserted) {
		d3.push_back(d1[i]);
		inserted = true;
	    }

	    if (d1[i].historical_num > d2[p2].historical_num and not inserted) {
		if (equal_fitness) { d3.push_back(d2[p2]); }
		++p2;
	    }

	}
    }
    return d3;
};


genome* breed(genome* g1, genome* g2) {
    if (g1->fitness < g2->fitness) {
	return breed(g2,g1);
    };
    std::cout << "Initiating breeding procedure between genome " << g1->ID << " and " << g2->ID << "...\n";
    std::vector<node*> g3_nod {};
    std::vector<dendrite> g3_den {};
    g3_nod = copy_cells(g1->cells, g2->cells);
    g3_den = cross(g1->dendrites, g2->dendrites, g1->fitness == g2->fitness);
    return new genome (g3_nod, g3_den);
};
