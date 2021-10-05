// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include "sys_conf.h"
#include "platform.h"

// PROBE Macros
#if defined (PROBE_PINS_ENABLED) && (PROBE_PINS_ENABLED == 1)
#define PROBE_GPIO_WRITE( gpio, n, x )  HAL_GPIO_WritePin( gpio, n, (GPIO_PinState)(x) )
#define PROBE_GPIO_SET_LINE( gpio, n )       LL_GPIO_SetOutputPin( gpio, n )
#define PROBE_GPIO_RST_LINE( gpio, n )       LL_GPIO_ResetOutputPin ( gpio, n )
// enabling RTC_OUTPUT should be set via STM32CubeMX GUI because MX_RTC_Init is now entirely generated
#elif defined (PROBE_PINS_ENABLED) && (PROBE_PINS_ENABLED == 0) /* PROBE_PINS_OFF */
#define PROBE_GPIO_WRITE( gpio, n, x )
#define PROBE_GPIO_SET_LINE( gpio, n )
#define PROBE_GPIO_RST_LINE( gpio, n )
// disabling RTC_OUTPUT should be set via STM32CubeMX GUI because MX_RTC_Init is now entirely generated
#else
#error "PROBE_PINS_ENABLED not defined or out of range <0,1>"
#endif

// Public

//  Disable debugger (serial wires pins)
void DBG_Disable(void);

// Config debugger when working in Low Power Mode
// Note: When in Dual Core DbgMcu pins should be better disable only after CM0+ is started
void DBG_ConfigForLpm(uint8_t enableDbg);

// Initializes the SW probes pins and the monitor RF pins via Alternate Function
void DBG_ProbesInit(void);
