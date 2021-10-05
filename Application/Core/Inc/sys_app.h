// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include "stdint.h"
#include "sys_conf.h"
#include "stm32_adv_trace.h"

// Macros
#define APP_PPRINTF(...)  do{ } while( UTIL_ADV_TRACE_OK \
                              != UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_OFF, __VA_ARGS__) ) /* Polling Mode */
#define APP_TPRINTF(...)   do{ {UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_ON, __VA_ARGS__);} }while(0); /* with timestamp */
#define APP_PRINTF(...)   do{ {UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_OFF, __VA_ARGS__);} }while(0);

#if defined (APP_LOG_ENABLED) && (APP_LOG_ENABLED == 1)
#define APP_LOG(TS,VL,...)   do{ {UTIL_ADV_TRACE_COND_FSend(VL, T_REG_OFF, TS, __VA_ARGS__);} }while(0);
#elif defined (APP_LOG_ENABLED) && (APP_LOG_ENABLED == 0) /* APP_LOG disabled */
#define APP_LOG(TS,VL,...)
#else
#error "APP_LOG_ENABLED not defined or out of range <0,1>"
#endif

// Public

// initialize the system (dbg pins, trace, mbmux, sys timer, LPM, ...)
void SystemApp_Init(void);

// callback to get the battery level in % of full charge (254 full charge, 0 no charge)
// Returns battery level
uint8_t GetBatteryLevel(void);

// callback to get the current temperature in the MCU
// Returns temperature level
uint16_t GetTemperatureLevel(void);

// callback to get the board 64 bits unique ID
// id unique ID
void GetUniqueId(uint8_t *id);

// callback to get the board 32 bits unique ID (LSB)
// devAddr Device Address
uint32_t GetDevAddr(void);

