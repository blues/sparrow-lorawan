// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include "platform.h"
#include "subghz.h"
#include "stm32_mem.h"       /* RADIO_MEMSET8 def in this file */
#include "mw_log_conf.h"     /* mw trace conf */
#include "board_radio.h"  /* low layer api (bsp) */
#include "utilities_def.h"  /* low layer api (bsp) */
#include "sys_debug.h"

// Radio debugging
#define DBG_GPIO_RADIO_RX(set_rst) PROBE_GPIO_##set_rst##_LINE(PROBE_LINE1_PORT, PROBE_LINE1_PIN);
#define DBG_GPIO_RADIO_TX(set_rst) PROBE_GPIO_##set_rst##_LINE(PROBE_LINE2_PORT, PROBE_LINE2_PIN);

// drive value used anytime radio is NOT in TX low power mode
// Note: override the default configuration of radio_driver.c
#define SMPS_DRIVE_SETTING_DEFAULT  SMPS_DRV_40

// drive value used anytime radio is in TX low power mode
//        TX low power mode is the worst case because the PA sinks from SMPS
//        while in high power mode, current is sunk directly from the battery
// Note:  override the default configuration of radio_driver.c
#define SMPS_DRIVE_SETTING_MAX      SMPS_DRV_60

// Provides the frequency of the chip running on the radio and the frequency step
// These defines are used for computing the frequency divider to set the RF frequency
// Note: override the default configuration of radio_driver.c
#define XTAL_FREQ                   ( 32000000UL )

// in XO mode, set internal capacitor (from 0x00 to 0x2F starting 11.2pF with 0.47pF steps)
// Note: override the default configuration of radio_driver.c
#define XTAL_DEFAULT_CAP_VALUE      ( 0x20UL )

// voltage of vdd tcxo.
// Note: override the default configuration of radio_driver.c
#define TCXO_CTRL_VOLTAGE           TCXO_CTRL_1_7V

// Radio maximum wakeup time (in ms)
// override the default configuration of radio_driver.c
#define RF_WAKEUP_TIME              ( 1UL )

// DCDC is enabled
// this define is only used if the DCDC is present on the board
// Note: override the default configuration of radio_driver.c
#define DCDC_ENABLE                 ( 1UL )

// Macros
#ifndef CRITICAL_SECTION_BEGIN
// macro used to enter the critical section
#define CRITICAL_SECTION_BEGIN( )      UTILS_ENTER_CRITICAL_SECTION( )
#endif
#ifndef CRITICAL_SECTION_END
#define CRITICAL_SECTION_END( )        UTILS_EXIT_CRITICAL_SECTION( )
#endif

// Function mapping
// SUBGHZ interface init to radio Middleware
#define RADIO_INIT                              MX_SUBGHZ_Init

// Delay interface to radio Middleware
#define RADIO_DELAY_MS                          HAL_Delay

// Memset utilities interface to radio Middleware
#define RADIO_MEMSET8( dest, value, size )      UTIL_MEM_set_8( dest, value, size )

// Memcpy utilities interface to radio Middleware
#define RADIO_MEMCPY8( dest, src, size )        UTIL_MEM_cpy_8( dest, src, size )
