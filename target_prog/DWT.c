// Copyright (C) 2021, Sorbonne Universite, LIP6
// This file is part of the LIP6DROMEL project, under the Apache License v2.0
// See https://www.apache.org/licenses/LICENSE-2.0 for license information.
// SPDX-License-Identifier: Apache-2.0

#include "DWT.h"

#include "hal.h"
#ifdef STM32F0
#include "stm32f0/stm32f0xx_hal_flash.h"
#endif
#ifdef STM32F1
#include "stm32f1/stm32f1xx_hal_flash.h"
#endif
#ifdef STM32F2
#include "stm32f2/stm32f2xx_hal_flash.h"
#endif
#ifdef STM32F3
#include "stm32f3/stm32f3xx_hal_flash.h"
#endif
#ifdef STM32F4
#include "stm32f4/stm32f4xx_hal_flash.h"
#endif

unsigned DWT_CycleCounter_init() {
  if (DWT->CTRL != 0) {
    CoreDebug->DEMCR |= 1U << CoreDebug_DEMCR_TRCENA_Pos;
    DWT->CYCCNT = 0;
    DWT->CTRL |= 1U << DWT_CTRL_CYCCNTENA_Pos;
    return 1;
  }

  return 0;
}

void DWT_CycleCounter_reset() { DWT->CYCCNT = 0; }

uint32_t DWT_CycleCounter_snapshot() { return DWT->CYCCNT; }

