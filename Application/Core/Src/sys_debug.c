// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "platform.h"
#include "sys_debug.h"

// Disable debugger
void DBG_Disable(void)
{

#if defined (DEBUGGER_ENABLED) && ( DEBUGGER_ENABLED == 0 ) /* DEBUGGER_DISABLED */

    // Put the debugger pin PA13 and P14 in analog for LowPower
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode   = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull   = GPIO_NOPULL;
    GPIO_InitStruct.Pin    = (GPIO_PIN_13 | GPIO_PIN_14);

    // make sure clock is enabled before setting the pins with HAL_GPIO_Init()
    __HAL_RCC_GPIOA_CLK_ENABLE() ;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

#elif !defined (DEBUGGER_ENABLED)

#error "DEBUGGER_ENABLED not defined or out of range <0,1>"

#endif // DEBUGGER_OFF

    // Disabled HAL_DBGMCU_
    DBG_ConfigForLpm(0);

}

// Config debugger when working in Low Power Mode
// Note:  When in Dual Core DbgMcu pins should be better disable only after Cm0 is started
void DBG_ConfigForLpm(uint8_t enableDbg)
{
    uint8_t enable_dbg = enableDbg;

#if defined (DEBUGGER_ENABLED) && ( DEBUGGER_ENABLED == 0 )

    enable_dbg = 0;

#elif !defined (DEBUGGER_ENABLED)

#error "DEBUGGER_ENABLED not defined or out of range <0,1>"

#endif // DEBUGGER_OFF

    if (enable_dbg == 1) {
        HAL_DBGMCU_EnableDBGSleepMode();
        HAL_DBGMCU_EnableDBGStopMode();
        HAL_DBGMCU_EnableDBGStandbyMode();
    } else {
        HAL_DBGMCU_DisableDBGSleepMode();
        HAL_DBGMCU_DisableDBGStopMode();
        HAL_DBGMCU_DisableDBGStandbyMode();
    }

}

// Init probes
void DBG_ProbesInit(void)
{

#if defined (PROBE_PINS_ENABLED) && ( PROBE_PINS_ENABLED == 1 )
    GPIO_InitTypeDef  GPIO_InitStruct = {0};

    // Configure the GPIO pin
    GPIO_InitStruct.Mode   = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull   = GPIO_PULLUP;
    GPIO_InitStruct.Speed  = GPIO_SPEED_FREQ_VERY_HIGH;

    // Enable the GPIO Clock
    PROBE_LINE1_CLK_ENABLE();
    PROBE_LINE2_CLK_ENABLE();
    PROBE_LINE3_CLK_ENABLE();
    PROBE_LINE4_CLK_ENABLE();

    GPIO_InitStruct.Pin    = PROBE_LINE1_PIN;
    HAL_GPIO_Init(PROBE_LINE1_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin    = PROBE_LINE2_PIN;
    HAL_GPIO_Init(PROBE_LINE2_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin    = PROBE_LINE3_PIN;
    HAL_GPIO_Init(PROBE_LINE3_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin    = PROBE_LINE4_PIN;
    HAL_GPIO_Init(PROBE_LINE4_PORT, &GPIO_InitStruct);

    // Reset probe Pins
    HAL_GPIO_WritePin(PROBE_LINE1_PORT, PROBE_LINE1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PROBE_LINE2_PORT, PROBE_LINE2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PROBE_LINE3_PORT, PROBE_LINE3_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PROBE_LINE4_PORT, PROBE_LINE4_PIN, GPIO_PIN_RESET);

    // HW alternate functions for monitoring RF

    // Configure the GPIO pin
    GPIO_InitStruct.Speed  = GPIO_SPEED_FREQ_VERY_HIGH;

    // spi dbg
    GPIO_InitStruct.Mode   = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull   = GPIO_NOPULL;
    GPIO_InitStruct.Pin    = (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIO_InitStruct.Alternate  = GPIO_AF13_DEBUG_SUBGHZSPI;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Busy
    GPIO_InitStruct.Mode   = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull   = GPIO_NOPULL;
    GPIO_InitStruct.Pin    = (GPIO_PIN_12);
    GPIO_InitStruct.Alternate  = GPIO_AF6_RF_BUSY;
    __HAL_RCC_GPIOA_CLK_ENABLE() ;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // LDO_rdy & BUCK_rdy (SMPS)
    GPIO_InitStruct.Mode   = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull   = GPIO_NOPULL;
    GPIO_InitStruct.Pin    = (GPIO_PIN_2);
    GPIO_InitStruct.Alternate  = GPIO_AF13_DEBUG_RF;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Mode   = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull   = GPIO_NOPULL;
    GPIO_InitStruct.Pin    = (GPIO_PIN_4);
    GPIO_InitStruct.Alternate  = GPIO_AF13_DEBUG_RF;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

#elif !defined (PROBE_PINS_ENABLED)

#error "PROBE_PINS_ENABLED not defined or out of range <0,1>"

#endif // PROBE_PINS_ENABLED

#if defined(CORE_CM4)
#if defined (DEBUGGER_ENABLED) && ( DEBUGGER_ENABLED == 1 )

    // Debug power up request wakeup CBDGPWRUPREQ
    LL_EXTI_EnableIT_32_63(LL_EXTI_LINE_46);

#elif !defined(DEBUGGER_ENABLED)

#error "DEBUGGER_ENABLED not defined or out of range <0,1>"

#endif // DEBUGGER_OFF
#endif // CORE_CM4

}
