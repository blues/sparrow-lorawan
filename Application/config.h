// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

// LoraWAN application configuration (Mw is configured by lorawan_conf.h)
#define CONFIG_ACTIVE_REGION    LORAMAC_REGION_US915

// From LoRaWAN Network Management Console
#define CONFIG_DEVEUI           { 0x60, 0x81, 0xF9, 0x4B, 0x02, 0x96, 0x4E, 0x2A }
#define CONFIG_APPEUI           { 0x60, 0x81, 0xF9, 0x5C, 0x7B, 0x60, 0xB0, 0x93  }
#define CONFIG_APPKEY           { 0x78, 0xA8, 0x18, 0x68, 0x20, 0x90, 0x8D, 0xAD, 0x56, 0xFD, 0x0E, 0x1A, 0xD3, 0x5E, 0xF3, 0x20 }
