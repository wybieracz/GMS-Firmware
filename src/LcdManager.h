#ifndef LCDMANAGER_H
#define LCDMANAGER_H

#include <LiquidCrystal.h>
#include "PinManager.h"
#include "FlashManager.h"

#define FREQ 5000
#define LED_CH 0
#define LED_RES 8

static int lcdBrightness = 255;

void initLCD();

bool setLcdBrightness(int brightness);

#endif //LCDMANAGER_H
