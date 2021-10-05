// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include <stdio.h>
#include "platform.h"
#include "sys_app.h"
#include "adc_if.h"
#include "stm32_seq.h"
#include "stm32_systime.h"
#include "stm32_lpm.h"
#include "timer_if.h"
#include "utilities_def.h"
#include "sys_debug.h"
#include "sys_sensors.h"

// Max timestamp size
#define MAX_TS_SIZE     ((int) 16)

// Maximum battery level
#define LORAWAN_MAX_BAT   254

// Forwards
static void TimestampNow(uint8_t *buff, uint16_t *size);
static void tiny_snprintf_like(char *buf, uint32_t maxsize, const char *strFormat, ...);

// Init SystemApp subsystem
void SystemApp_Init(void)
{

    // Ensure that MSI is wake-up system clock
    __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);

    // Initialize timer and RTC
    UTIL_TIMER_Init();

    // Debug config : disable serial wires and DbgMcu pins settings
    DBG_Disable();

    // Initializes the SW probes pins and the monitor RF pins via Alternate Function
    DBG_ProbesInit();

    // Initialize the terminal
    UTIL_ADV_TRACE_Init();
    UTIL_ADV_TRACE_RegisterTimeStampFunction(TimestampNow);

    // Set verbose LEVEL
    UTIL_ADV_TRACE_SetVerboseLevel(VERBOSE_LEVEL);

    // Initialize the temperature and Battery measurement services
    SYS_InitMeasurement();

    // Initialize the Sensors
    EnvSensors_Init();

    // Init low power manager
    UTIL_LPM_Init();

    // Disable Stand-by mode
    UTIL_LPM_SetOffMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);

#if defined (LOW_POWER_DISABLE) && (LOW_POWER_DISABLE == 1)
    // Disable Stop Mode
    UTIL_LPM_SetStopMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);
#elif !defined (LOW_POWER_DISABLE)
#error LOW_POWER_DISABLE not defined
#endif // LOW_POWER_DISABLE

}

// redefines __weak function in stm32_seq.c such to enter low power
void UTIL_SEQ_Idle(void)
{
    UTIL_LPM_EnterLowPower();
}

// get battery voltage
uint8_t GetBatteryLevel(void)
{
    uint8_t batteryLevel = 0;
    uint16_t batteryLevelmV;

    batteryLevelmV = (uint16_t) SYS_GetBatteryLevel();

    // Convert battery level from mV to linear scale: 1 (very low) to 254 (fully charged)
    if (batteryLevelmV > VDD_BAT) {
        batteryLevel = LORAWAN_MAX_BAT;
    } else if (batteryLevelmV < VDD_MIN) {
        batteryLevel = 0;
    } else {
        batteryLevel = (((uint32_t)(batteryLevelmV - VDD_MIN) * LORAWAN_MAX_BAT) / (VDD_BAT - VDD_MIN));
    }

    APP_LOG(TS_ON, VLEVEL_M, "VDDA= %d\r\n", batteryLevel);

    return batteryLevel;  // 1 (very low) to 254 (fully charged)
}

// get temp
uint16_t GetTemperatureLevel(void)
{
    uint16_t temperatureLevel = 0;
    temperatureLevel = (uint16_t)(SYS_GetTemperatureLevel() / 256);
    return temperatureLevel;
}

//
void GetUniqueId(uint8_t *id)
{
    uint32_t val = 0;
    val = LL_FLASH_GetUDN();
    if (val == 0xFFFFFFFF) { // Normally this should not happen
        uint32_t ID_1_3_val = HAL_GetUIDw0() + HAL_GetUIDw2();
        uint32_t ID_2_val = HAL_GetUIDw1();

        id[7] = (ID_1_3_val) >> 24;
        id[6] = (ID_1_3_val) >> 16;
        id[5] = (ID_1_3_val) >> 8;
        id[4] = (ID_1_3_val);
        id[3] = (ID_2_val) >> 24;
        id[2] = (ID_2_val) >> 16;
        id[1] = (ID_2_val) >> 8;
        id[0] = (ID_2_val);
    } else { // Typical use case
        id[7] = val & 0xFF;
        id[6] = (val >> 8) & 0xFF;
        id[5] = (val >> 16) & 0xFF;
        id[4] = (val >> 24) & 0xFF;
        val = LL_FLASH_GetDeviceID();
        id[3] = val & 0xFF;
        val = LL_FLASH_GetSTCompanyID();
        id[2] = val & 0xFF;
        id[1] = (val >> 8) & 0xFF;
        id[0] = (val >> 16) & 0xFF;
    }

}

// Get the device address
uint32_t GetDevAddr(void)
{
    uint32_t val = 0;

    val = LL_FLASH_GetUDN();
    if (val == 0xFFFFFFFF) {
        val = ((HAL_GetUIDw0()) ^ (HAL_GetUIDw1()) ^ (HAL_GetUIDw2()));
    }

    return val;

}

// Returns sec and msec based on the systime in use
// buff to update with timestamp
// size of updated buffer
static void TimestampNow(uint8_t *buff, uint16_t *size)
{
    SysTime_t curtime = SysTimeGet();
    tiny_snprintf_like((char *)buff, MAX_TS_SIZE, "%ds%03d:", curtime.Seconds, curtime.SubSeconds);
    *size = strlen((char *)buff);
}

// Disable StopMode when traces need to be printed
void UTIL_ADV_TRACE_PreSendHook(void)
{
    UTIL_LPM_SetStopMode((1 << CFG_LPM_UART_TX_Id), UTIL_LPM_DISABLE);
}

// Re-enable StopMode when traces have been printed
void UTIL_ADV_TRACE_PostSendHook(void)
{
    UTIL_LPM_SetStopMode((1 << CFG_LPM_UART_TX_Id), UTIL_LPM_ENABLE);
}

// UTIL_ADV_TRACE_VSNPRINTF wrapper
static void tiny_snprintf_like(char *buf, uint32_t maxsize, const char *strFormat, ...)
{
    va_list vaArgs;
    va_start(vaArgs, strFormat);
    UTIL_ADV_TRACE_VSNPRINTF(buf, maxsize, strFormat, vaArgs);
    va_end(vaArgs);
}


// HAL overrides for tick handling
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    // Don't enable SysTick if TIMER_IF is based on other counters (e.g. RTC)
    return HAL_OK;
}
uint32_t HAL_GetTick(void)
{
    // TIMER_IF can be based on other counter the SysTick e.g. RTC
    return TIMER_IF_GetTimerValue();
}
void HAL_Delay(__IO uint32_t Delay)
{
    // TIMER_IF can be based on other counter the SysTick e.g. RTC
    TIMER_IF_DelayMs(Delay);
}
