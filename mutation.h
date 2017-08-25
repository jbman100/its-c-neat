#include "objects.h"

void mutate_weight(genome*);

void mutate_add_node(genome*);

std::vector<std::pair<std::pair<int,int>,int>> mutate_add_connection(genome*, std::vector<std::pair<std::pair<int,int>,int>>);

void mutate(std::vector<genome*>);
