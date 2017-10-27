#include "mutation.h"

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Mutations /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////// Weight mutation ///////////////////////////////////////////////


void mutate_weight(genome* specimen) {
    int instance;
    int index_conn = specimen->dendrites.size();
    for (int i = specimen->first_output; i < specimen->first_hidden; ++i) {
	instance = rand() %100;
	if (instance < changeWeightProb) {
	    specimen->cells[i]->bias = 20 - (float)(rand() %200)/5;
	}
    }
//    for (int i = specimen->first_hidden; i < (int)specimen->cells.size(); ++i) {
//	instance = rand() %100;
//	if (instance < weight_prob) {
//	    specimen->cells[i]->bias = 20 - (float)(rand() %200)/5;
//	}
//    }
    for (int i = 0; i < index_conn; ++i) {
	instance = rand() %100;
	if (instance < changeWeightProb) {
	    specimen->dendrites[i].weight = 20 - (float)(rand() %200)/5;
	}
    }
}


/////////////////////////////// Adding a node /////////////////////////////////////////////////


std::vector<std::pair<int,int>> mutate_add_node(genome* specimen, std::vector<std::pair<int,int>> record) {
    int instance = rand() %100;
    if (instance < addNodeProb) {
	std::cout << "Adding new node to genome ID n:" << specimen->ID << " ...\n";
	int n = specimen->dendrites.size();
	std::vector<std::pair<int,int>> existing;
	for (int i = 0; i < n; ++i) {		    //List existing connections
	    if (specimen->dendrites[i].enabled) {
		std::pair<int,int> connection;
		connection.first = specimen->dendrites[i].origin;
		connection.second = specimen->dendrites[i].dest;
		existing.push_back(connection);
	    }
	}
	n = existing.size();
	if (n > 0) {
	    int choice = rand() %n;
	    std::pair<int,int> to_replace = existing[choice];
	    specimen->dendrites[choice].enabled = false;
	    bool original = true;

	    for (int i = 0; i < (int)record.size(); ++i) {
		if (record[i].first == specimen->dendrites[choice].historical_num) {
		    original = false;
		    int target_nid = record[i].second;
		    specimen->cells.push_back(new node (target_nid));
		    specimen->dendrites.push_back(dendrite(to_replace.first, target_nid, 1));
		    specimen->dendrites.push_back(dendrite(target_nid, to_replace.second, specimen->dendrites[choice-1].weight));
		}
	    }

	    if (original) {
		record.push_back(std::make_pair(specimen->dendrites[choice].historical_num,nid));
		specimen->cells.push_back(new node (nid,0,'h'));
		specimen->dendrites.push_back(dendrite(to_replace.first, nid, 1));
		specimen->dendrites.push_back(dendrite(nid, to_replace.second, specimen->dendrites[choice-1].weight));
		++nid;
	    }

	    std::cout << "Genome gained a node. NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN\n";
	} else {
	    std::cout << "No connections could be replaced.\n";
	}
    }
    return record;
}


/////////////////////////////// Adding a dendrite /////////////////////////////////////////////


std::pair<int,int> random_pair(std::vector<int> first_members, std::vector<int> second_members) {
    std::pair<int,int> choice;
    int a = rand() %(int)first_members.size(), b = rand() %(int)second_members.size();
    while (first_members[a] == second_members[b]) { b = rand() %(int)second_members.size(); };
    std::cout << "Dendrite going from cell " << first_members[a] << " to cell " << second_members[b] << " selected.\n";
    return std::make_pair(first_members[a],second_members[b]);
}


std::vector<std::pair<std::pair<int,int>,int>> mutate_add_connection(genome* specimen, std::vector<std::pair<std::pair<int,int>,int>> record) {
    float instance = rand()%100;
    if (instance < addDendriteProb) {
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
	    for (int i = 0; i < specimen->first_output; ++i) { fm.push_back(i); std::cout << i << " is an input\n";}
	    std::vector<int> sm {};
	    for (int i = specimen->first_output; i < specimen->first_hidden; ++i) { sm.push_back(i); std::cout << i << " is an output\n";}
	    for (int i = specimen->first_hidden; i < (int)specimen->cells.size(); ++i) {
		fm.push_back(specimen->cells[i]->number);
		sm.push_back(specimen->cells[i]->number);
	    }
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
		    }
		}
	    }

	    n = record.size();
	    int hnum = -1;
	    for (int i = 0; i < n; ++i) {
		std::pair<int,int> connection = record[i].first;
		if (potent.first == connection.first and potent.second == connection.second) {
		    hnum = record[i].second;
		}
	    }

	    float weight = 20 - (float)(rand() %200)/5;

	    if (hnum == -1) {
		std::pair<std::pair<int,int>,int> new_record = std::make_pair(std::make_pair(potent.first,potent.second),Hnum);
		record.push_back(new_record);
		specimen->dendrites.push_back(dendrite(potent.first,potent.second,weight));
	    } else {
		specimen->dendrites.push_back(dendrite(hnum,potent.first,potent.second,weight));
	    }
	    std::cout << "Genome gained a dendrite. DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD\n";
	}
    }
    return record;
}


/////////////////////////////// Mutation procedure ////////////////////////////////////////////


void mutate(std::vector<genome*> offspring) {
    std::vector<std::pair<std::pair<int,int>,int>> record_dendrites;
    std::vector<std::pair<int,int>> record_nodes;
    for (int i = 0; i < (int)offspring.size(); ++i) {
	mutate_weight(offspring[i]);
	record_dendrites = mutate_add_connection(offspring[i],record_dendrites);
	record_nodes = mutate_add_node(offspring[i], record_nodes);
    }
}
