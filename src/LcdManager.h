#ifndef LCDMANAGER_H
#define LCDMANAGER_H

#include <LiquidCrystal.h>
#include "PinManager.h"
#include "FlashManager.h"

#define FREQ 10000
#define LED_CH 0
#define LED_RES 8

static int lcdBrightness = 255;

void initLCD();

bool setLcdBrightness(int brightness);

void lcdPrint(char* text, int x, int y);

void lcdClear();

#endif //LCDMANAGER_H
