// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "app_lorawan.h"
#include "lora_app.h"
#include "sys_app.h"
#include "stm32_seq.h"

// Init LoRaWAN
void MX_LoRaWAN_Init(void)
{
    SystemApp_Init();
    LoRaWAN_Init();
}

// Main processing function
void MX_LoRaWAN_Process(void)
{
    UTIL_SEQ_Run(UTIL_SEQ_DEFAULT);
}
