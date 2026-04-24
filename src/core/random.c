#include "random.h"
#include <stdlib.h>
#include <time.h>

/**
 * Sets the random seed for reproducibility.
 * If not called, defaults to srand(1) — always the same sequence.
 * 
 * @param seed: Any u32 value. Use random_set_seed_time() for random behavior.
 */
void random_set_seed(u32 seed) {
    srand(seed);
}


/**
 * Sets the random seed based on the current time.
 * This ensures different sequences of random numbers on each run.
 */
void random_set_seed_time() {
    srand((u32)time(NULL));
}