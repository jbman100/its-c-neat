#include "breeding.h"
#include "mutation.h"
#include <algorithm>
#include <iostream>

//                   _       _   _               _     
//    _____   _____ | |_   _| |_(_) ___  _ __   | |__  
//   / _ \ \ / / _ \| | | | | __| |/ _ \| '_ \  | '_ | 
//  |  __/\ V / (_) | | |_| | |_| | (_) | | | |_| | | |
//   \___| \_/ \___/|_|\__,_|\__|_|\___/|_| |_(_)_| |_|
                                                   

float compatibility(genome*, genome*, int, int, int);
void categorize(genome*, std::vector<species*>*);
void iterate(std::vector<species*>*);
