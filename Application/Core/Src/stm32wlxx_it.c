// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "main.h"
#include "stm32wlxx_it.h"

// Handles
extern RTC_HandleTypeDef hrtc;
extern SUBGHZ_HandleTypeDef hsubghz;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern UART_HandleTypeDef huart2;

// Cortex core interrupt handlers
void NMI_Handler(void)
{
    while (1) {
    }
}
void HardFault_Handler(void)
{
    while (1) {
    }
}
void MemManage_Handler(void)
{
    while (1) {
    }
}
void BusFault_Handler(void)
{
    while (1) {
    }
}
void UsageFault_Handler(void)
{
    while (1) {
    }
}
void SVC_Handler(void)
{
}
void DebugMon_Handler(void)
{
}
void PendSV_Handler(void)
{
}
void SysTick_Handler(void)
{
}

// Peripheral interrupt handlers
// please refer to the startup file (startup_stm32wlxx.s)

// This function handles RTC Tamper, RTC TimeStamp, LSECSS and RTC SSRU Interrupts.
void TAMP_STAMP_LSECSS_SSRU_IRQHandler(void)
{
    HAL_RTCEx_SSRUIRQHandler(&hrtc);
}

// EXTI Handlers
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

// DMA Handlers
void DMA1_Channel5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart2_tx);
}

// USART2
void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);
}

// RTC Alarms (A and B) Interrupt.
void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&hrtc);
}

// SubGHZ Radio Interrupt
void SUBGHZ_Radio_IRQHandler(void)
{
    HAL_SUBGHZ_IRQHandler(&hsubghz);
}

