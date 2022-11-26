// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#ifndef SERIALLOGGER_H
#define SERIALLOGGER_H

#include <Arduino.h>

#ifndef SERIAL_LOGGER_BAUD_RATE
#define SERIAL_LOGGER_BAUD_RATE 115200
#endif

class SerialLogger {
  private:
    void writeTime();
  public:
    SerialLogger();
    void info(String message);
    void error(String message);
};

extern SerialLogger logger;

#endif // SERIALLOGGER_H