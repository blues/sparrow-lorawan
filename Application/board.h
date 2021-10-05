// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#define RTC_N_PREDIV_S 10
#define RTC_PREDIV_S ((1<<RTC_N_PREDIV_S)-1)
#define RTC_PREDIV_A ((1<<(15-RTC_N_PREDIV_S))-1)
#define LED1_Pin GPIO_PIN_15
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_9
#define LED2_GPIO_Port GPIOB
#define PROB3_Pin GPIO_PIN_14
#define PROB3_GPIO_Port GPIOB
#define RF_CTRL3_Pin GPIO_PIN_3
#define RF_CTRL3_GPIO_Port GPIOC
#define BUT1_Pin GPIO_PIN_0
#define BUT1_GPIO_Port GPIOA
#define BUT1_EXTI_IRQn EXTI0_IRQn
#define PROB2_Pin GPIO_PIN_13
#define PROB2_GPIO_Port GPIOB
#define RF_CTRL2_Pin GPIO_PIN_5
#define RF_CTRL2_GPIO_Port GPIOC
#define PROB1_Pin GPIO_PIN_12
#define PROB1_GPIO_Port GPIOB
#define RF_CTRL1_Pin GPIO_PIN_4
#define RF_CTRL1_GPIO_Port GPIOC
#define BUT3_Pin GPIO_PIN_6
#define BUT3_GPIO_Port GPIOC
#define BUT2_Pin GPIO_PIN_1
#define BUT2_GPIO_Port GPIOA
#define BUT2_EXTI_IRQn EXTI1_IRQn
#define LED3_Pin GPIO_PIN_11
#define LED3_GPIO_Port GPIOB
#define USARTx_RX_Pin GPIO_PIN_3
#define USARTx_RX_GPIO_Port GPIOA
#define USARTx_TX_Pin GPIO_PIN_2
#define USARTx_TX_GPIO_Port GPIOA
#define PROB4_Pin GPIO_PIN_10
#define PROB4_GPIO_Port GPIOB

#define USE_BSP_DRIVER

#define PROBE_LINE1_PIN                           GPIO_PIN_12
#define PROBE_LINE1_PORT                          GPIOB
#define PROBE_LINE1_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()
#define PROBE_LINE1_CLK_DISABLE()                 __HAL_RCC_GPIOB_CLK_DISABLE()

#define PROBE_LINE2_PIN                           GPIO_PIN_13
#define PROBE_LINE2_PORT                          GPIOB
#define PROBE_LINE2_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()
#define PROBE_LINE2_CLK_DISABLE()                 __HAL_RCC_GPIOB_CLK_DISABLE()

#define PROBE_LINE3_PIN                           GPIO_PIN_14
#define PROBE_LINE3_PORT                          GPIOB
#define PROBE_LINE3_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()
#define PROBE_LINE3_CLK_DISABLE()                 __HAL_RCC_GPIOB_CLK_DISABLE()

#define PROBE_LINE4_PIN                           GPIO_PIN_10
#define PROBE_LINE4_PORT                          GPIOB
#define PROBE_LINE4_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()
#define PROBE_LINE4_CLK_DISABLE()                 __HAL_RCC_GPIOB_CLK_DISABLE()
