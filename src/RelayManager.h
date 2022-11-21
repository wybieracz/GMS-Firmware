#ifndef RELAYMANAGER_H
#define RELAYMANAGER_H

//#include <Arduino.h>
#include "PinManager.h"
#include "FlashManager.h"

extern bool relayState;

void initRelay();

void openRelay();

void closeRelay();

#endif //RELAYMANAGER_H