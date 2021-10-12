// Copyright 2021 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "sensor.h"

// Measure sensor values and retrieve data to be transmitted, returning true if success or false if failure.
bool sensorGetData(uint8_t *buf, uint8_t bufmaxlen, uint8_t *buflen, uint32_t *secsSuccess, uint32_t *secsFailure)
{

    // Set callback timings on either success or failure to send this sensor measurement
    *secsFailure = 10;
    *secsSuccess = 15;

    // Append simulated sensor data, carefully noting that the code must match the template supplied
    // when configuring the integration in either ttn or helium as follows.  (Note that there is also
    // an X-Hub header field which can be set to an alternate notehub, which defaults to: api.notefile.net
    // For template format see:
    // https://dev.blues.io/notecard/notecard-walkthrough/low-bandwidth-design/#understanding-template-data-types
    //
    // Integration Configuration within The Things Network or Helium console
    //   https://notecard.live/lorawan
	//   X-Session-Token: <your project's notehub API session token>
	//   X-Format: ttn <or helium>
	//   X-Product: <any product UID within your project>
	//   X-Template: {"a":11,"b":12,"c":13,"d":14,"temp":12.1,"lat":14.1,"lon":14.1}
    //
    binBegin(buf, bufmaxlen, 0);
    binAppendInt8(0);               // "a"
    binAppendInt16(1);              // "b"
    binAppendInt24(2);              // "c"
    binAppendInt32(3);              // "d"
    binAppendReal16(-1.23);         // "temp"
    binAppendReal32(4.56);          // "lat"
    binAppendReal32(7.89);          // "lon"
    *buflen = binEnd();

    // Done
    APP_PRINTF("sensor retrieved %d bytes\r\n", *buflen);
    return true;

}
