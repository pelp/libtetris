#ifndef LCG_RAND_H
#define LCG_RAND_H

// Linear Congruential Generator (LCG) implementation
void     lcg_srand48(long int seedval);
long int lcg_lrand48(void);

#endif // LCG_RAND_H