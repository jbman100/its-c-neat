#include "objects.h"

extern int max_population;

//   _                       _ _               _     
//  | |__  _ __ ___  ___  __| (_)_ __   __ _  | |__  
//  | '_ \| '__/ _ \/ _ \/ _` | | '_ \ / _` | | '_ | 
//  | |_) | | |  __/  __/ (_| | | | | | (_| |_| | | |
//  |_.__/|_|  \___|\___|\__,_|_|_| |_|\__, (_)_| |_|
//                                     |___/         

std::vector<node*> copy_cells(std::vector<node*>, std::vector<node*>);
std::vector<dendrite> cross(std::vector<dendrite>, std::vector<dendrite>, bool);
genome* breed(genome*, genome*);
