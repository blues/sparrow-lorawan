// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "sensor.h"

// Measure sensor values and retrieve data to be transmitted, returning
// true if success or false if failure.
bool sensorGetData(uint8_t *buf, uint8_t bufmaxlen, uint8_t *buflen, uint32_t *secsSuccess, uint32_t *secsFailure)
{

    // Set timings for success and failure
    *secsFailure = 10;
    *secsSuccess = 15;

    // Append simulated sensor data, matching the template
    // {"a":11,"b":12,"c":13,"d":14,"temp":12.1,"lat":14.1,"lon":14.1}
    binBegin(buf, bufmaxlen, 0);
    binAppendInt8(0);
    binAppendInt16(1);
    binAppendInt24(2);
    binAppendInt32(3);
    binAppendReal16(-1.23);
    binAppendReal32(4.56);
    binAppendReal32(7.89);
    *buflen = binEnd();

    APP_PRINTF("sensor retrieved %d bytes\r\n", *buflen);
    return true;

}
