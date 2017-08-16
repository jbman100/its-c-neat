#include "objects.h"
#include <iostream>

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
