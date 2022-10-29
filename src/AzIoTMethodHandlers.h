#ifndef AZIOTMETHODHANDLERS_H
#define AZIOTMETHODHANDLERS_H

#include <Arduino.h>
//#include "LedController.h"

#define LED_GREEN 32

extern bool telemetryEnabled;

bool enableTelemetry(int value);
// bool setColor(char* ptr);
// bool setMode(int value);
// bool getStatus(char* ptr);

#endif //AZIOTMETHODHANDLERS_H