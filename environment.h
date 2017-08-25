#include "objects.h"
#include <iostream>
#include <algorithm>

int max_population = 10;
float excessParam = 1, disjointParam = 1, avWeightParam = 0.5, compatibilityThreshold = 1;


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

	    if ((d1[i].historical_num < d2[p2].historical_num and not inserted) or p2 == n2) {
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
    std::cout << "Compability check:\n";

    int N;
    if (g1->dendrites.size() > g2->dendrites.size()) { N = g1->dendrites.size(); }
    else { N = g2->dendrites.size(); }
    std::cout << "N calculated\n";
    
    std::vector<std::vector<int>> indexes = sort_dendrites(g1->dendrites, g2->dendrites);
    std::cout << "Dendrites sorted.\n";
    int E = indexes[1].size(), D = indexes[2].size();
    if (verbose) {
	std::cout << "E: " << E << std::endl
	    << "D: " << D << std::endl
	    << "n: " << indexes[0].size() << std::endl;
    }
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

    float result = (float)(c1*E + c2*D)/N + c3*W/n;
    if (verbose) {
	std::cout << "W: " << W << std::endl;
	std::cout << "Compatibility factor between genome "
	    << g1->ID << " and genome "
	    << g2->ID << " equals " 
	    << result << std::endl;
    }
    return result;
}


void categorize(genome* specimen, std::vector<species*>* population) {
    std::cout << "Inserting genome " << specimen->ID << " in population...\n";
    int n = population->size();
    bool inserted = false;
    for (int i = 0; i < n; ++i) {
	std::cout << "Comparing compatibility with genome " << (*population)[i]->alpha_g->ID << "...\n";
	if (compatibility(specimen, (*population)[i]->alpha_g, excessParam, disjointParam, avWeightParam) < compatibilityThreshold and not inserted) {
	    specimen->species = i;
	    (*population)[i]->members.push_back(specimen);
	    if (verbose) {std::cout << "Genome " << specimen->ID << " inserted in species " << i << "\n";}
	    inserted = true;
	}
    }
    if (not inserted) {
	population->push_back(new species(specimen));
	population->back()->members[0]->species = n;
	if (verbose) {std::cout << "New species created.\n";}
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Genetic backbone //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


bool compare_genomes(genome* g1, genome* g2) {
    if (g1->fitness > g2->fitness) { return true; }
    else { return false; }
}


void sort_species(species* target) {
    std::sort(target->members.begin(),target->members.end(),compare_genomes);
}


void iterate(std::vector<species*>* population) {
    int n = population->size();
    std::vector<genome*> offspring = {};
    bool isWinnerSaved = false;

    for (int i = 0; i < n; ++i) {
	sort_species((*population)[i]);				    // Sort the species
	(*population)[i]->alpha_g = (*population)[i]->members[0];   // Set the new reference
	if ((*population)[i]->members.size() > 5) {
	    offspring.push_back((*population)[i]->alpha_g);
	    isWinnerSaved = true;
	}
    }

    std::cout << "Step 1 done: sorting and setting references." << std::endl;

    while ((int)offspring.size() < max_population) {
	for (int i = 0; i < n; ++i) {
	    //int species_card = (*population)[i]->members.size()-1;
	    genome* specimen1 = (*population)[i]->members[0];
	    genome* specimen2 = (*population)[i]->members[1];
	    std::cout << "Generating child n:" << offspring.size() << std::endl;
	    offspring.push_back(breed(specimen1, specimen2));	    //TODO selection proportionnal to the fitness
	}
    }

    std::cout << "Step 2 done: generating offspring." << std::endl;
    
    std::cout << "step 3 done" << std::endl;

    for (int i = 0; i < n; ++i) {
	int species_card = (*population)[i]->members.size()-1;
	for (int j = species_card-1; j > 0; --j) {		    // Clear old genomes
	    if (j == 0 and not isWinnerSaved) { kill((*population)[i]->members[j]); }
	    std::cout << "Genome killed." << std::endl;
	    (*population)[i]->members.pop_back();
	}
	(*population)[i]->members.pop_back();
    }

    std::cout << "step 4 done" << std::endl;

    for (int i = 0; i < max_population; ++i) {
	build(offspring[i]);
	categorize(offspring[i], population);
    }
}
