// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "usart_if.h"

// Handles
extern DMA_HandleTypeDef hdma_usart2_tx;
extern UART_HandleTypeDef huart2;

// Character buffer
uint8_t charRx;

// Trace driver callbacks handler
const UTIL_ADV_TRACE_Driver_s UTIL_TraceDriver = {
    vcom_Init,
    vcom_DeInit,
    vcom_ReceiveInit,
    vcom_Trace_DMA,
};

//  Completion callbacks
static void (*TxCpltCallback)(void *);
static void (*RxCpltCallback)(uint8_t *rxChar, uint16_t size, uint8_t error);

// Init virtual comms
UTIL_ADV_TRACE_Status_t vcom_Init(void (*cb)(void *))
{
    TxCpltCallback = cb;
    MX_DMA_Init();
    MX_USART2_UART_Init();
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_27);
    return UTIL_ADV_TRACE_OK;
}

// DeInit virtual comms
UTIL_ADV_TRACE_Status_t vcom_DeInit(void)
{

    // ## 1 Reset peripherals
    __HAL_RCC_USART2_FORCE_RESET();
    __HAL_RCC_USART2_RELEASE_RESET();

    // ## 2 MspDeInit
    HAL_UART_MspDeInit(&huart2);

    // ## 3 Disable the NVIC for DMA
    HAL_NVIC_DisableIRQ(DMA1_Channel5_IRQn);

    return UTIL_ADV_TRACE_OK;

}

// Trace over the vcom port with interrupts
void vcom_Trace(uint8_t *p_data, uint16_t size)
{
    HAL_UART_Transmit(&huart2, p_data, size, 1000);
}

// Trace with DMA
UTIL_ADV_TRACE_Status_t vcom_Trace_DMA(uint8_t *p_data, uint16_t size)
{
    HAL_UART_Transmit_DMA(&huart2, p_data, size);
    return UTIL_ADV_TRACE_OK;
}

// Receive
UTIL_ADV_TRACE_Status_t vcom_ReceiveInit(void (*RxCb)(uint8_t *rxChar, uint16_t size, uint8_t error))
{
    UART_WakeUpTypeDef WakeUpSelection;

    // record call back
    RxCpltCallback = RxCb;

    // Set wakeUp event on start bit
    WakeUpSelection.WakeUpEvent = UART_WAKEUP_ON_STARTBIT;
    HAL_UARTEx_StopModeWakeUpSourceConfig(&huart2, WakeUpSelection);

    // Make sure that no UART transfer is on-going
    while (__HAL_UART_GET_FLAG(&huart2, USART_ISR_BUSY) == SET);

    // Make sure that UART is ready to receive)
    while (__HAL_UART_GET_FLAG(&huart2, USART_ISR_REACK) == RESET);

    // Enable USART interrupt
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_WUF);

    // Enable wakeup from stop mode
    HAL_UARTEx_EnableStopMode(&huart2);

    // Start LPUART receive on IT
    HAL_UART_Receive_IT(&huart2, &charRx, 1);

    return UTIL_ADV_TRACE_OK;

}

// Resume vcom after the stm32 being stopped
void vcom_Resume(void)
{

    // to re-enable lost UART settings
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }

    // to re-enable lost DMA settings
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK) {
        Error_Handler();
    }

}

// Transmit complete callback
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart2)
{
    TxCpltCallback(NULL);
}

// Receive complete callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart2)
{
    if ((NULL != RxCpltCallback) && (HAL_UART_ERROR_NONE == huart2->ErrorCode)) {
        RxCpltCallback(&charRx, 1, 0);
    }
    HAL_UART_Receive_IT(huart2, &charRx, 1);
}
