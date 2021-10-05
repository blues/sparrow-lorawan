// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Applicative configuration, e.g. : debug, trace, low power, sensors
#pragma once

// Exported constants

// Temperature and pressure values are retrieved from sensors shield
// (instead of sending dummy values). It requires MEMS IKS shield
#define SENSOR_ENABLED              0

// Verbose level for all trace logs
#define VERBOSE_LEVEL               VLEVEL_M

// Enable trace logs
#define APP_LOG_ENABLED             1

// Enable MCU Debugger pins (dbg serial wires, sbg spi, etc)
#define DEBUGGER_ENABLED            0

// Enable four wires usable as probes (two of them PROBE1 and PROBE2 used by the MW)
#define PROBE_PINS_ENABLED          0

// Disable Low Power mode
// Note: 0: LowPowerMode enabled. MCU enters stop2 mode, 1: LowPowerMode disabled. MCU enters sleep mode only
#define LOW_POWER_DISABLE           0
