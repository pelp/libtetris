#include "lcg_rand.h"

#define LCG_A 0x5DEECE66DL
#define LCG_C 0xBL
#define LCG_M ((1LL << 48) - 1)

// The state of the LCG
static unsigned long long int lcg_state = 0x1234ABCD330E;

// Seed the random number generator
void lcg_srand48(long int seedval) {
    lcg_state = (seedval << 16) | 0x330E;
}

// Generate a random number
long int lcg_lrand48(void) {
    lcg_state = (LCG_A * lcg_state + LCG_C) & LCG_M;
    return (long int)(lcg_state >> 17);
}