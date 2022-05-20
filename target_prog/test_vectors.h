// Copyright (C) 2021, Sorbonne Universite, LIP6
// This file is part of the LIP6DROMEL project, under the Apache License v2.0
// See https://www.apache.org/licenses/LICENSE-2.0 for license information.
// SPDX-License-Identifier: Apache-2.0

#ifndef TEST_VECTORS_H
#define TEST_VECTORS_H

#include "MT_RNG.h"

#include <stdint.h>

#define reset(r) ({ __asm__ __volatile__ (\
					  "eor %[reg], %[reg], %[reg]"\
					  : [reg] "=r" (r) :[reg] "r" (r)); })

#define nop ({ __asm__ __volatile__ ("eor r12, r12, r12"); })
#define nop4 ({ nop; nop; nop; nop; })
#define nop16 ({ nop4; nop4; nop4; nop4; })

#define realnop ({ __asm__ __volatile__ ("nop"); })
#define realnop4 ({ realnop; realnop; realnop; realnop; })
#define realnop16 ({ realnop4; realnop4; realnop4; realnop4; })

#define nop_param3(x,y,z) ({ __asm__ __volatile__ (\
						   "eor %[a], %[b], %[c] " \
						   :[a] "=r" (x):[b] "r" (y), [c] "r" (z)); })
#define nop4_param3(x,y,z) ({ \
      nop_param3(x,y,z);			\
      nop_param3(x,y,z);			\
      nop_param3(x,y,z);			\
      nop_param3(x,y,z); })

#define nop16_param3(x,y,z) ({ nop4_param3(x,y,z); \
      nop4_param3(x,y,z);			   \
      nop4_param3(x,y,z);			   \
      nop4_param3(x,y,z); })

#define push_reg __asm__ __volatile__("push {r4, r5, r6, r7, r8, r9, r10, r11}":::)
#define pop_reg  __asm__ __volatile__("pop {r4, r5, r6, r7, r8, r9, r10, r11}":::)

#define NUM_INPUTS 4
#define NUM_OUTPUTS 4
#define NUM_KEYS 4

extern uint32_t Out[NUM_INPUTS];
extern uint32_t In[NUM_OUTPUTS];
extern uint32_t Key[NUM_KEYS];
extern MTRand rng;

void trigger_high(void);
void trigger_low(void);

// This function contains the test vector one wants to run. The in and out
// buffers are filled with data by server.py, they are used to provide test
// data or random values for clearing the processor state. Any value to be
// returned has to be written to the out buffer.
void test_vector(uint32_t *in, uint32_t *out);

// Define this function together with test_vector if some initializations have
// to be performed before entering the main command loop. This is ran once,
// only when the board is reseted. A default weak definition of an empty
// function will be used if not overriden.
void test_vector_initialization();

// Define this function together with test_vector is you need to be able to run
// some function. A typical usage would be to update the key with the buffer
// in argument before running many iterations of test vector (with carying input
// data). test_vector_update would be used to initialize the AES context structure
// which is then persistent over the many iterations. A default weak definition
// of an empty function will be used if not overriden.
void test_vector_update(uint32_t *key);

// This function is invoked when server.py issues the 'o' command. The default
// implementation is defined as weak, will transmit the Out buffer, so you can
// override it with your own implementation together with your test_vector
// function.
void test_vector_send_buffer();

// This function is invoked to seed the random number generator when server.py
// issues the 'm' command. The default implementation will seed the global rng,
// but this can be overriden if needed. When overriden, rng will be default
// seeded.
void test_vector_seed_rng(uint32_t seed);

#endif // TEST_VECTORS_H
