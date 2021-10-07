// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "stdint.h"
#include "config_sys.h"
#include "sys_sensors.h"

#define STSOP_LATTITUDE           ((float) 43.618622 )  /*!< default latitude position */
#define STSOP_LONGITUDE           ((float) 7.051415  )  /*!< default longitude position */
#define MAX_GPS_POS               ((int32_t) 8388607 )  /*!< 2^23 - 1 */
#define HUMIDITY_DEFAULT_VAL      50.0f                 /*!< default humidity */
#define TEMPERATURE_DEFAULT_VAL   18.0f                 /*!< default temperature */
#define PRESSURE_DEFAULT_VAL      1000.0f               /*!< default pressure */

// Read sensors
int32_t  EnvSensors_Read(sensor_t *sensor_data)
{
    float HUMIDITY_Value = HUMIDITY_DEFAULT_VAL;
    float TEMPERATURE_Value = TEMPERATURE_DEFAULT_VAL;
    float PRESSURE_Value = PRESSURE_DEFAULT_VAL;

    sensor_data->humidity    = HUMIDITY_Value;
    sensor_data->temperature = TEMPERATURE_Value;
    sensor_data->pressure    = PRESSURE_Value;

    sensor_data->latitude  = (int32_t)((STSOP_LATTITUDE  * MAX_GPS_POS) / 90);
    sensor_data->longitude = (int32_t)((STSOP_LONGITUDE  * MAX_GPS_POS) / 180);

    return 0;

}

int32_t  EnvSensors_Init(void)
{
    return 0;
}
