// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Low layer function to enter/exit low power modes (stop, sleep)

#include "platform.h"
#include "stm32_lpm.h"
#include "stm32_lpm_if.h"
#include "usart_if.h"

// Power driver callbacks handler
const struct UTIL_LPM_Driver_s UTIL_PowerDriver = {
    PWR_EnterSleepMode,
    PWR_ExitSleepMode,
    PWR_EnterStopMode,
    PWR_ExitStopMode,
    PWR_EnterOffMode,
    PWR_ExitOffMode,
};

void PWR_EnterOffMode(void)
{
}

void PWR_ExitOffMode(void)
{
}

void PWR_EnterStopMode(void)
{
    HAL_SuspendTick();

    // Clear Status Flag before entering STOP/STANDBY Mode
    LL_PWR_ClearFlag_C1STOP_C1STB();

    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);

}

void PWR_ExitStopMode(void)
{

    // Resume sysTick : work around for debugger problem in dual core
    HAL_ResumeTick();

    // Not retained periph:
    //  ADC interface
    //  DAC interface USARTx, TIMx, i2Cx, SPIx
    //  SRAM ctrls, DMAx, DMAMux, AES, RNG, HSEM

    // Resume not retained USARTx and DMA
    vcom_Resume();

}

void PWR_EnterSleepMode(void)
{
    HAL_SuspendTick();
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

void PWR_ExitSleepMode(void)
{
    HAL_ResumeTick();
}

