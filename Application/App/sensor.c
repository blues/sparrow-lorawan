// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "app.h"

// Measure sensor values and retrieve data to be transmitted, returning
// true if success or false if failure.
bool sensorGetData(uint8_t *buf, uint8_t bufmaxlen, uint8_t *buflen, uint32_t *secsSuccess, uint32_t *secsFailure)
{

    // Set timings for success and failure
    *secsFailure = 10;
    *secsSuccess = 15;

    // Append simulated sensor data
    binBegin(buf, bufmaxlen, 0);
    binAppendInt8(1);
    binAppendInt16(2);
    binAppendInt32(3);
    *buflen = binEnd();

    APP_PRINTF("sensor retrieved %d bytes\r\n", *buflen);
    return true;

}
