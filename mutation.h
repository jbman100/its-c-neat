#include "objects.h"

//                   _        _   _               _     
//   _ __ ___  _   _| |_ __ _| |_(_) ___  _ __   | |__  
//  | '_ ` _ \| | | | __/ _` | __| |/ _ \| '_ \  | '_ | 
//  | | | | | | |_| | || (_| | |_| | (_) | | | |_| | | |
//  |_| |_| |_|\__,_|\__\__,_|\__|_|\___/|_| |_(_)_| |_|
                                                    

void mutate_weight(genome*);
std::vector<std::pair<int,int>> mutate_add_node(genome*, std::vector<std::pair<int,int>>);
std::vector<std::pair<std::pair<int,int>,int>> mutate_add_connection(genome*, std::vector<std::pair<std::pair<int,int>,int>>);
void mutate(std::vector<genome*>);
