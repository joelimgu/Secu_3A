/*
 * This implementation of a Mersenne Twister is derived from
 * https://github.com/ESultanik/mtwister
 */
#ifndef MT_RNG_H
#define MT_RNG_H

#include <stdint.h>

#define STATE_VECTOR_LENGTH 624

typedef struct {
  uint32_t mt[STATE_VECTOR_LENGTH];
  int32_t index;
} MTRand;

/**
 * Initializes a random number generator from a given seed.
 */
void seedRand(MTRand *r, uint32_t seed);

/**
 * Generates a pseudo-randomly generated long.
 */
uint32_t genRandLong(MTRand *rand);

#endif
