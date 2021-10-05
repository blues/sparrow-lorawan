// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "platform.h"
#include "Region.h"     // Needed for LORAWAN_DEFAULT_DATA_RATE
#include "sys_app.h"
#include "lora_app.h"
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "utilities_def.h"
#include "lora_app_version.h"
#include "lorawan_version.h"
#include "subghz_phy_version.h"
#include "lora_info.h"
#include "LmHandler.h"
#include "stm32_lpm.h"
#include "adc_if.h"
#include "sys_conf.h"
#include "sys_sensors.h"

// Privates

// LoRa State Machine states
typedef enum TxEventType_e {
    // Appdata Transmission issue based on timer every TxDutyCycleTime
    TX_ON_TIMER,
    // Appdata Transmission external event plugged on OnSendEvent( )
    TX_ON_EVENT
} TxEventType_t;


static void SendTxData(void);
static void OnTxTimerEvent(void *context);
static void OnJoinRequest(LmHandlerJoinParams_t *joinParams);
static void OnTxData(LmHandlerTxParams_t *params);
static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params);
static void OnMacProcessNotify(void);
static void OnTxTimerLedEvent(void *context);
static void OnRxTimerLedEvent(void *context);
static void OnJoinTimerLedEvent(void *context);

// Private vars
static ActivationType_t ActivationType = LORAWAN_DEFAULT_ACTIVATION_TYPE;

// LoRaWAN handler Callbacks
static LmHandlerCallbacks_t LmHandlerCallbacks = {
    .GetBatteryLevel =           GetBatteryLevel,
    .GetTemperature =            GetTemperatureLevel,
    .GetUniqueId =               GetUniqueId,
    .GetDevAddr =                GetDevAddr,
    .OnMacProcess =              OnMacProcessNotify,
    .OnJoinRequest =             OnJoinRequest,
    .OnTxData =                  OnTxData,
    .OnRxData =                  OnRxData
};

// LoRaWAN handler parameters
static LmHandlerParams_t LmHandlerParams = {
    .ActiveRegion =             ACTIVE_REGION,
    .DefaultClass =             LORAWAN_DEFAULT_CLASS,
    .AdrEnable =                LORAWAN_ADR_STATE,
    .TxDatarate =               LORAWAN_DEFAULT_DATA_RATE,
    .PingPeriodicity =          LORAWAN_DEFAULT_PING_SLOT_PERIODICITY
};

// Type of Event to generate application Tx
static TxEventType_t EventType = TX_ON_TIMER;

// Timer to handle the application Tx
static UTIL_TIMER_Object_t TxTimer;

// User application buffer
static uint8_t AppDataBuffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];

// User application data structure
static LmHandlerAppData_t AppData = { 0, 0, AppDataBuffer };

// Specifies the state of the application LED
static uint8_t AppLedStateOn = RESET;

// Timer to handle the application Tx Led to toggle
static UTIL_TIMER_Object_t TxLedTimer;

// Timer to handle the application Rx Led to toggle
static UTIL_TIMER_Object_t RxLedTimer;

// Timer to handle the application Join Led to toggle
static UTIL_TIMER_Object_t JoinLedTimer;

// Init lorawan
void LoRaWAN_Init(void)
{

    BSP_LED_Init(LED_BLUE);
    BSP_LED_Init(LED_GREEN);
    BSP_LED_Init(LED_RED);
    BSP_PB_Init(BUTTON_SW2, BUTTON_MODE_EXTI);

    // Get LoRa APP version
    APP_LOG(TS_OFF, VLEVEL_M, "APP_VERSION:        V%X.%X.%X\r\n",
            (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_MAIN_SHIFT),
            (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_SUB1_SHIFT),
            (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_SUB2_SHIFT));

    // Get MW LoraWAN info
    APP_LOG(TS_OFF, VLEVEL_M, "MW_LORAWAN_VERSION: V%X.%X.%X\r\n",
            (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_MAIN_SHIFT),
            (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_SUB1_SHIFT),
            (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_SUB2_SHIFT));

    // Get MW SubGhz_Phy info
    APP_LOG(TS_OFF, VLEVEL_M, "MW_RADIO_VERSION:   V%X.%X.%X\r\n",
            (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_MAIN_SHIFT),
            (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_SUB1_SHIFT),
            (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_SUB2_SHIFT));

    UTIL_TIMER_Create(&TxLedTimer, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTxTimerLedEvent, NULL);
    UTIL_TIMER_Create(&RxLedTimer, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnRxTimerLedEvent, NULL);
    UTIL_TIMER_Create(&JoinLedTimer, 0xFFFFFFFFU, UTIL_TIMER_PERIODIC, OnJoinTimerLedEvent, NULL);
    UTIL_TIMER_SetPeriod(&TxLedTimer, 500);
    UTIL_TIMER_SetPeriod(&RxLedTimer, 500);
    UTIL_TIMER_SetPeriod(&JoinLedTimer, 500);

    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LmHandlerProcess), UTIL_SEQ_RFU, LmHandlerProcess);
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), UTIL_SEQ_RFU, SendTxData);

    // Init Info table used by LmHandler
    LoraInfo_Init();

    // Init the Lora Stack
    LmHandlerInit(&LmHandlerCallbacks);
    LmHandlerConfigure(&LmHandlerParams);

    UTIL_TIMER_Start(&JoinLedTimer);

    LmHandlerJoin(ActivationType);

    if (EventType == TX_ON_TIMER) {
        // send every time timer elapses
        UTIL_TIMER_Create(&TxTimer,  0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTxTimerEvent, NULL);
        UTIL_TIMER_SetPeriod(&TxTimer,  APP_TX_DUTYCYCLE);
        UTIL_TIMER_Start(&TxTimer);
    } else {
        // send every time button is pushed
        BSP_PB_Init(BUTTON_SW1, BUTTON_MODE_EXTI);
    }

}

// Note: Current the stm32wlxx_it.c generated by STM32CubeMX does not support BSP for PB in EXTI mode.
// In order to get a push button IRS by code automatically generated
// HAL_GPIO_EXTI_Callback is today the only available possibility.
// Using HAL_GPIO_EXTI_Callback() shortcuts the BSP.
// If users wants to go through the BSP, stm32wlxx_it.c should be updated 
// in the USER CODE SESSION of the correspondent EXTIn_IRQHandler()
// to call the BSP_PB_IRQHandler() or the HAL_EXTI_IRQHandler(&H_EXTI_n);.
// Then the below HAL_GPIO_EXTI_Callback() can be replaced by BSP callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin) {
    case  BUTTON_SW1_PIN:
        // Note: when "EventType == TX_ON_TIMER" this GPIO is not initialized
        UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);
        break;
    case  BUTTON_SW2_PIN:
        break;
    case  BUTTON_SW3_PIN:
        break;
    default:
        break;
    }
}

// callback when LoRa application has received a frame
// appData data received in the last Rx
// params status of last Rx
static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{

    if ((appData != NULL) || (params != NULL)) {
        BSP_LED_On(LED_BLUE) ;

        UTIL_TIMER_Start(&RxLedTimer);

        static const char *slotStrings[] = { "1", "2", "C", "C Multicast", "B Ping-Slot", "B Multicast Ping-Slot" };

        APP_LOG(TS_OFF, VLEVEL_M, "\r\n###### ========== MCPS-Indication ==========\r\n");
        APP_LOG(TS_OFF, VLEVEL_H, "###### D/L FRAME:%04d | SLOT:%s | PORT:%d | DR:%d | RSSI:%d | SNR:%d\r\n",
                params->DownlinkCounter, slotStrings[params->RxSlot], appData->Port, params->Datarate, params->Rssi, params->Snr);
        switch (appData->Port) {
        case LORAWAN_SWITCH_CLASS_PORT:
            // this port switches the class
            if (appData->BufferSize == 1) {
                switch (appData->Buffer[0]) {
                case 0: {
                    LmHandlerRequestClass(CLASS_A);
                    break;
                }
                case 1: {
                    LmHandlerRequestClass(CLASS_B);
                    break;
                }
                case 2: {
                    LmHandlerRequestClass(CLASS_C);
                    break;
                }
                default:
                    break;
                }
            }
            break;
        case LORAWAN_USER_APP_PORT:
            if (appData->BufferSize == 1) {
                AppLedStateOn = appData->Buffer[0] & 0x01;
                if (AppLedStateOn == RESET) {
                    APP_LOG(TS_OFF, VLEVEL_H,   "LED OFF\r\n");
                    BSP_LED_Off(LED_RED) ;
                } else {
                    APP_LOG(TS_OFF, VLEVEL_H, "LED ON\r\n");
                    BSP_LED_On(LED_RED) ;
                }
            }
            break;

        default:

            break;
        }
    }

}

// LoRa End Node send request
static void SendTxData(void)
{

    uint16_t pressure = 0;
    int16_t temperature = 0;
    sensor_t sensor_data;
    UTIL_TIMER_Time_t nextTxIn = 0;

    uint16_t humidity = 0;
    uint32_t i = 0;
    int32_t latitude = 0;
    int32_t longitude = 0;
    uint16_t altitudeGps = 0;

    EnvSensors_Read(&sensor_data);
    temperature = (SYS_GetTemperatureLevel() >> 8);
    pressure    = (uint16_t)(sensor_data.pressure * 100 / 10);      /* in hPa / 10 */

    AppData.Port = LORAWAN_USER_APP_PORT;

    humidity    = (uint16_t)(sensor_data.humidity * 10);            /* in %*10     */

    AppData.Buffer[i++] = AppLedStateOn;
    AppData.Buffer[i++] = (uint8_t)((pressure >> 8) & 0xFF);
    AppData.Buffer[i++] = (uint8_t)(pressure & 0xFF);
    AppData.Buffer[i++] = (uint8_t)(temperature & 0xFF);
    AppData.Buffer[i++] = (uint8_t)((humidity >> 8) & 0xFF);
    AppData.Buffer[i++] = (uint8_t)(humidity & 0xFF);

    if ((LmHandlerParams.ActiveRegion == LORAMAC_REGION_US915) || (LmHandlerParams.ActiveRegion == LORAMAC_REGION_AU915)
            || (LmHandlerParams.ActiveRegion == LORAMAC_REGION_AS923)) {
        AppData.Buffer[i++] = 0;
        AppData.Buffer[i++] = 0;
        AppData.Buffer[i++] = 0;
        AppData.Buffer[i++] = 0;
    } else {
        latitude = sensor_data.latitude;
        longitude = sensor_data.longitude;

        AppData.Buffer[i++] = GetBatteryLevel();        /* 1 (very low) to 254 (fully charged) */
        AppData.Buffer[i++] = (uint8_t)((latitude >> 16) & 0xFF);
        AppData.Buffer[i++] = (uint8_t)((latitude >> 8) & 0xFF);
        AppData.Buffer[i++] = (uint8_t)(latitude & 0xFF);
        AppData.Buffer[i++] = (uint8_t)((longitude >> 16) & 0xFF);
        AppData.Buffer[i++] = (uint8_t)((longitude >> 8) & 0xFF);
        AppData.Buffer[i++] = (uint8_t)(longitude & 0xFF);
        AppData.Buffer[i++] = (uint8_t)((altitudeGps >> 8) & 0xFF);
        AppData.Buffer[i++] = (uint8_t)(altitudeGps & 0xFF);
    }

    AppData.BufferSize = i;

    if (LORAMAC_HANDLER_SUCCESS == LmHandlerSend(&AppData, LORAWAN_DEFAULT_CONFIRMED_MSG_STATE, &nextTxIn, false)) {
        APP_LOG(TS_ON, VLEVEL_L, "SEND REQUEST\r\n");
    } else if (nextTxIn > 0) {
        APP_LOG(TS_ON, VLEVEL_L, "Next Tx in  : ~%d second(s)\r\n", (nextTxIn / 1000));
    }

}

// TX timer callback function
// context ptr of timer context
static void OnTxTimerEvent(void *context)
{
    UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);
    // Wait for next tx slot
    UTIL_TIMER_Start(&TxTimer);
}

// LED Tx timer callback function
// context ptr of LED context
static void OnTxTimerLedEvent(void *context)
{
    BSP_LED_Off(LED_GREEN) ;
}

// LED Rx timer callback function
// context ptr of LED context
static void OnRxTimerLedEvent(void *context)
{
    BSP_LED_Off(LED_BLUE) ;
}

// LED Join timer callback function
// context ptr of LED context
static void OnJoinTimerLedEvent(void *context)
{
    BSP_LED_Toggle(LED_RED) ;
}

// tx event callback function
// params status of last Tx
static void OnTxData(LmHandlerTxParams_t *params)
{
    if ((params != NULL)) {
        // Process Tx event only if its a mcps response to prevent some internal events (mlme) 
        if (params->IsMcpsConfirm != 0) {
            BSP_LED_On(LED_GREEN) ;
            UTIL_TIMER_Start(&TxLedTimer);

            APP_LOG(TS_OFF, VLEVEL_M, "\r\n###### ========== MCPS-Confirm =============\r\n");
            APP_LOG(TS_OFF, VLEVEL_H, "###### U/L FRAME:%04d | PORT:%d | DR:%d | PWR:%d", params->UplinkCounter,
                    params->AppData.Port, params->Datarate, params->TxPower);

            APP_LOG(TS_OFF, VLEVEL_H, " | MSG TYPE:");
            if (params->MsgType == LORAMAC_HANDLER_CONFIRMED_MSG) {
                APP_LOG(TS_OFF, VLEVEL_H, "CONFIRMED [%s]\r\n", (params->AckReceived != 0) ? "ACK" : "NACK");
            } else {
                APP_LOG(TS_OFF, VLEVEL_H, "UNCONFIRMED\r\n");
            }
        }
    }
}

// join event callback function
// joinParams status of join
static void OnJoinRequest(LmHandlerJoinParams_t *joinParams)
{
    if (joinParams != NULL) {
        if (joinParams->Status == LORAMAC_HANDLER_SUCCESS) {
            UTIL_TIMER_Stop(&JoinLedTimer);
            BSP_LED_Off(LED_RED) ;

            APP_LOG(TS_OFF, VLEVEL_M, "\r\n###### = JOINED = ");
            if (joinParams->Mode == ACTIVATION_TYPE_ABP) {
                APP_LOG(TS_OFF, VLEVEL_M, "ABP ======================\r\n");
            } else {
                APP_LOG(TS_OFF, VLEVEL_M, "OTAA =====================\r\n");
            }
        } else {
            APP_LOG(TS_OFF, VLEVEL_M, "\r\n###### = JOIN FAILED\r\n");
        }
    }
}

// Will be called each time a Radio IRQ is handled by the MAC layer
static void OnMacProcessNotify(void)
{
    UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LmHandlerProcess), CFG_SEQ_Prio_0);
}
