#include "objects.h"


///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Node class ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



float node::transfer(float input) {
    float out = (1/(1+std::exp(input*-4)));		    // A simple transfer function.

    if (out > 0.9) { return 1; }
    else { return 0; }
};


float node::get_value(bool verbose) {

    if (node::evaluated) {			    // If previously evaluated: just output the value
	std::cout << "Node " << number << " has a value of " << value << std::endl;
	return value;
    }
    
    else {
	value = 0;

	for (int i = 0; i < (int)peers.size(); ++i) {
	    value = value + (*peers[i].first).get_value(verbose)*(peers[i].second);
	}

	if (verbose) {std::cout << "Node " << number <<
	    "'s value is " << value + bias <<
		" so it outputs " << transfer(value + bias)
		<< "\n";
	}

	value = transfer(value + bias);
	evaluated = true;
	return value;
    };
};



///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Genome methods ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



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

	for (int i = specimen->first_output; i < (int)specimen->cells.size(); ++i) {
	    specimen->cells[i]->evaluated = false;
	};
    };
    return OU;
};


float calc_fit(genome* specimen, std::vector<std::vector<int>> input, std::vector<std::vector<int>> ref, std::vector<species*>* population) {
    std::vector<std::vector<int>> result = test_input(specimen, input);
    int n = ref.size(), out = ref[0].size();
    float fitness = 4;
    for (int i = 0; i < n; ++i) {
	int sum = 0;
	for (int j = 0; j < out; ++j) {
	    sum = sum + ref[i][j] - result[i][j];
	};
	fitness = fitness - sqrt(pow(sum,2));
    };
    return pow(fitness,2)/(*population)[specimen->species]->members.size();
};

///////////////////////////////////////////////////////////////////////////////////////////////


void species::calc_average() {
    float sum = 0.0;
    int n = members.size();
    for (int i = 0; i < n; ++i) {
	sum = sum + members[i]->fitness;
    }
    average_fitness = sum/n;
}

void coitus_allowance(pop population, float population_average){
    int n = population->size();
    for (int i = 0; i < n; ++i) {
	(*population)[i]->allowed_offspring = (int)(*population)[i]->average_fitness/population_average;
    }
}
