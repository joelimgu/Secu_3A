// Copyright (C) 2021, Sorbonne Universite, LIP6
// This file is part of the LIP6DROMEL project, under the Apache License v2.0
// See https://www.apache.org/licenses/LICENSE-2.0 for license information.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stdint.h>

unsigned DWT_CycleCounter_init();
void DWT_CycleCounter_reset();
uint32_t DWT_CycleCounter_snapshot();
