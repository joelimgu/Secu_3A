/*
 * The implementation is derived from https://github.com/ESultanik/mtwister.
 * It has just been refactored with clangd, types have been changed to use
 * explicitely-sized types, and a few memory accesses have been removed.
 *
 */

#include "MT_RNG.h"

#define STATE_VECTOR_M 397
#define UPPER_MASK 0x80000000UL
#define LOWER_MASK 0x7fffffffUL
#define TEMPERING_MASK_B 0x9d2c5680UL
#define TEMPERING_MASK_C 0xefc60000UL

void seedRand(MTRand *rand, uint32_t seed) {
    /* Set initial seeds to mt[STATE_VECTOR_LENGTH] using the generator
     * from Line 25 of Table 1 in: Donald Knuth, "The Art of Computer
     * Programming," Vol. 2 (2nd Ed.) pp.102.
     */
    rand->mt[0] = seed & 0xffffffff;
    for (uint32_t i = 1; i < STATE_VECTOR_LENGTH; i++) {
        rand->mt[i] = (6069 * rand->mt[i - 1]) & 0xffffffff;
    }
    rand->index = STATE_VECTOR_LENGTH;
}

uint32_t genRandLong(MTRand * rand) {
    uint32_t y;
    static const uint32_t mag[2] = { 0x0, 0x9908b0df }; /* mag[x] = x * 0x9908b0df for x = 0,1 */

    if (rand->index >= STATE_VECTOR_LENGTH || rand->index < 0) {
        /* generate STATE_VECTOR_LENGTH words at a time */
        if (rand->index >= STATE_VECTOR_LENGTH + 1 || rand->index < 0) {
            seedRand(rand, 4357);
        }
        uint32_t kk;
        for (kk = 0; kk < STATE_VECTOR_LENGTH - STATE_VECTOR_M; kk += 1) {
            y = (rand->mt[kk] & UPPER_MASK) | (rand->mt[kk + 1] & LOWER_MASK);
            rand->mt[kk] = rand->mt[kk + STATE_VECTOR_M] ^ (y >> 1) ^ mag[y & 0x1];
        }
        for (; kk < STATE_VECTOR_LENGTH - 1; kk += 1) {
            y = (rand->mt[kk] & UPPER_MASK) | (rand->mt[kk + 1] & LOWER_MASK);
            rand->mt[kk] = rand->mt[kk + (STATE_VECTOR_M - STATE_VECTOR_LENGTH)] ^ (y >> 1) ^ mag[y & 0x1];
        }
        y = (rand->mt[STATE_VECTOR_LENGTH - 1] & UPPER_MASK) | (rand->mt[0] & LOWER_MASK);
        rand->mt[STATE_VECTOR_LENGTH - 1] = rand->mt[STATE_VECTOR_M - 1] ^ (y >> 1) ^ mag[y & 0x1];
        rand->index = 0;
    }

    y = rand->mt[rand->index++];
    y ^= (y >> 11);
    y ^= (y << 7) & TEMPERING_MASK_B;
    y ^= (y << 15) & TEMPERING_MASK_C;
    y ^= (y >> 18);

    return y;
}

