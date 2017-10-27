#include "evolution.h"

float excessParam = 1, disjointParam = 1, avWeightParam = 0.5, compatibilityThreshold = 1;


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
	if (not inserted and compatibility(specimen, (*population)[i]->alpha_g, excessParam, disjointParam, avWeightParam) < compatibilityThreshold) {
	    specimen->species = i;
	    (*population)[i]->members.push_back(specimen);
	    if (verbose) {std::cout << "Genome " << specimen->ID << " inserted in species " << i << "\n\n";}
	    inserted = true;
	}
    }
    if (not inserted) {
	population->push_back(new species(specimen));
	population->back()->members[0]->species = n;
	if (verbose) {std::cout << "New species created.\n\n";}
    }
}


bool compare_genomes(genome* g1, genome* g2) {
    if (g1->fitness > g2->fitness) { return true; }
    else { return false; }
}


void prep_species(species* sample) {
    std::sort(sample->members.begin(), sample->members.end(), compare_genomes);
    sample->alpha_g = sample->members[0];
    sample->calc_average();
}


float calc_population_average(std::vector<species*>* population) {
    float sum = 0;
    int population_card = 0;

    for (int i = 0; i < (int)population->size(); ++i) {
	int n = (*population)[i]->members.size();
	for (int j = 0; j < n; ++j) {
	    ++population_card;
	    sum = sum + (*population)[i]->members[j]->fitness;
	}
    }
   return(sum/population_card);
}



///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Genetic backbone //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



void iterate(std::vector<species*>* population) {
    int n = population->size();
    std::vector<genome*> offspring = {};

    for (int i = 0; i < n; ++i) {
	prep_species((*population)[i]);
	if ((*population)[i]->members.size() >= 5) {
	    offspring.push_back((*population)[i]->alpha_g);
	}
    }
    
    //population_average = calc_population_average(population);

    std::cout << "Step 1 done: sorting and setting references. ################################" << std::endl;

    for (int i = 0; i < n; ++i) {
	for (int j = 0; j < (*population)[i]->allowed_offspring; ++j) {
	    int species_card = (*population)[i]->members.size();
	    genome* specimen1 = (*population)[i]->members[rand()%species_card];
	    genome* specimen2 = (*population)[i]->members[rand()%species_card];
	    std::cout << "Generating child n:" << offspring.size() << std::endl;
	    offspring.push_back(breed(specimen1, specimen2));	    //TODO selection proportionnal to the fitness
	}
    }

    std::cout << "Step 2 done: generating offspring. ##########################################" << std::endl;
    
    mutate(offspring);

    std::cout << "Step 3 done: offspring mutation #############################################" << std::endl;

    for (int i = 0; i < n; ++i) {
	int species_card = (*population)[i]->members.size()-1;
	for (int j = species_card-1; j > 0; --j) {		    // Clear old genomes
	    kill((*population)[i]->members[j]);
	    std::cout << "Genome killed." << std::endl;
	    (*population)[i]->members.pop_back();
	}
	while ((int)(*population)[i]->members.size() > 0) {
	    (*population)[i]->members.pop_back();
	}
    }

    std::cout << "Step 4 done: clearing old population ########################################" << std::endl;

    std::cout << "There are " << offspring.size() << " offspring.\n";
    for (int i = 0; i < (int)offspring.size(); ++i) {
	build(offspring[i]);
	std::cout << "Offspring number " << i << " built.\n";
	categorize(offspring[i], population);
    }
    n = population->size();
    for (int i = n-1; i >= 0; --i) {
	if ((*population)[i]->members.size() == 0) {
	    delete (*population)[i];
	    population->erase(population->begin()+i);
	}
    }
}
