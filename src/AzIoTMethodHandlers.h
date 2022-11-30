#ifndef AZIOTMETHODHANDLERS_H
#define AZIOTMETHODHANDLERS_H

#include <Arduino.h>
#include "PinManager.h"
#include "RelayManager.h"

extern bool telemetryEnabled;

bool enableTelemetry(char value);
bool toggleRelay(int value);
// bool setColor(char* ptr);
// bool getStatus(char* ptr);

#endif //AZIOTMETHODHANDLERS_H