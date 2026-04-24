#ifndef RANDOM_H
#define RANDOM_H

#include "types.h"

void random_set_seed(u32 seed);  // fixed seed
void random_set_seed_time();     // seed based on current time
#endif