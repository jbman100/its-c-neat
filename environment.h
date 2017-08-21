#include "objects.h"
#include <iostream>



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
		out.insert(it + p1 -1,c2[i]);
		inserted = true;
	    };
	    if (p1 == n1-1 and not inserted) {
		out.push_back(c2[i]);
		inserted = true;
	    };
	    ++p1;
	};
    };
    return out;
}


std::vector<dendrite> cross(std::vector<dendrite> d1, std::vector<dendrite> d2){
    std::vector<dendrite> d3;
    int n1 = d1.size();
    for (int i = 0; i < n1; ++i) {
	bool inserted = false;
	int p2 = 0;
	while (not inserted) {
	    if (d1[i].historical_num == d2[p2].historical_num) {
		int choice = rand() %2;
		if (choice == 0) {
		    d3.push_back(d1[i]);
		} else {
		    d3.push_back(d2[p2]);
		}
		++p2;
		inserted = true;
	    };
	    if (d1[i].historical_num > d2[p2].historical_num and not inserted) {
		++p2;
	    }
	    if (d1[i].historical_num < d2[p2].historical_num and not inserted) {
		d3.push_back(d1[i]);
		inserted = true;
	    }
	}
    }
    return d3;
};


genome* breed(genome* g1, genome* g2) {
    if (g1->fitness < g2->fitness) {
	return breed(g2,g1);
    };
    std::vector<node*> g3_nod {};
    std::vector<dendrite> g3_den {};
    g3_nod = copy_cells(g1->cells,g2->cells);
    g3_den = cross(g1->dendrites,g2->dendrites);
    return new genome (g3_nod,g3_den);
};



///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Mutations /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////// Weight mutation ///////////////////////////////////////////////


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


/////////////////////////////// Adding a node /////////////////////////////////////////////////


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


/////////////////////////////// Adding a dendrite /////////////////////////////////////////////


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



///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Speciation procedure //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



std::vector<std::vector<int>> sort_dendrites (std::vector<dendrite> d1, std::vector<dendrite> d2) { 

    if (d1.back().historical_num < d2.back().historical_num) {
	return sort_dendrites(d2,d1);
    }

    std::vector<int> matching;
    std::vector<int> excess;
    std::vector<int> disjoint;
    int p1 = 0, n1 = d1.size(), n2 = d2.size();
    for (int i = 0; i < n2; ++i) { 
        bool inserted = false;
        while (not inserted and p1 < n1) { 

            if (d1[p1].historical_num == d2[i].historical_num) { 
                matching.push_back(d1[p1].historical_num);
                ++p1;
                inserted = true;
            } 

            if (d1[p1].historical_num > d2[i].historical_num and not inserted) { 
                disjoint.push_back(d2[i].historical_num);
                inserted = true;
            } 

            if (d1[p1].historical_num < d2[i].historical_num and not inserted) { 
                disjoint.push_back(d1[p1].historical_num);
                ++p1;
            } 
        } 
    } 
    for (int i = p1; i < n1; ++i) { 
        excess.push_back(d1[i].historical_num);
    } 
    return {matching,excess,disjoint};
}


float compatibility (genome* g1, genome* g2, int c1, int c2, int c3) {

    int N;
    if (g1->dendrites.size() > g2->dendrites.size()) { N = g1->dendrites.size(); }
    else { N = g2->dendrites.size(); }
    
    std::vector<std::vector<int>> indexes = sort_dendrites(g1->dendrites, g2->dendrites);
    int E = indexes[1].size(), D = indexes[2].size();

    float W = 0;
    int n = indexes[0].size();
    for (int i = 0; i < n; ++i) {
	int w1,w2;
	for (int j = 0; j < (int)g1->dendrites.size(); ++j) {
	    if (g1->dendrites[j].historical_num == indexes[0][i]) { w1 = g1->dendrites[j].weight; }
	}
	for (int j = 0; j < (int)g2->dendrites.size(); ++j) {
	    if (g2->dendrites[j].historical_num == indexes[0][i]) { w2 = g2->dendrites[j].weight; }
	}
	W = W + std::abs(w2-w1);
    }

    W = W/n;
    return (c1*E + c2*D)/N + c3*W;
}


void categorize(genome* specimen, std::vector<species*>* population) {
    int n = population->size();
    bool inserted = false;
    if (n > 0) {
	for (int i = 0; i < n; ++i) {
	    if (compatibility(specimen, (*population)[i]->alpha_g, exw, diw, avw) < delta and not inserted) {
		specimen->species = i;
		(*population)[i]->members.push_back(specimen);
		std::cout << "Genome inserted in species " << i << "\n";
		inserted = true;
	    }
	}
    }
    if (not inserted) { population->push_back(new species(specimen)); std::cout << "New species created.\n"; }
}
