// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

// Sensor data parameters
typedef struct {
    float pressure;         /*!< in mbar */
    float temperature;      /*!< in degC */
    float humidity;         /*!< in % */
    int32_t latitude;       /*!< latitude converted to binary */
    int32_t longitude ;     /*!< longitude converted to binary */
    int16_t altitudeGps;    /*!< in m */
    int16_t altitudeBar ;   /*!< in m * 10 */
} sensor_t;

#if defined (SENSOR_ENABLED) && (SENSOR_ENABLED == 1) && defined (X_NUCLEO_IKS01A2)
#define HTS221_0    0U
#define LPS22HB_0   1U
#endif

// initialize the environmental sensor
int32_t EnvSensors_Init(void);

// Environmental sensor  read.
// sensor_data sensor data
int32_t EnvSensors_Read(sensor_t *sensor_data);

