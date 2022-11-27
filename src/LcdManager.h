#ifndef LCDMANAGER_H
#define LCDMANAGER_H

#include <LiquidCrystal.h>
#include "PinManager.h"
#include "FlashManager.h"

#define FREQ 10000
#define LED_CH 0
#define LED_RES 8

class LcdManager {
  private:
    String brightnessMem;
    int brightness;
    String settingsMem;
    char settings[3];
  public:
  LcdManager();
  void init();
  bool setBrightness(char* data);
  bool setDisplay(char* data);
  char* getSettings();
  void print(char* text, int x, int y);
  void printFloat(float value, int precision, int x, int y);
  void clear();
};

extern LcdManager lcdManager;

// static int brightness = 255;

// void initLCD();

// bool setLcdBrightness(int brightness);

// void lcdPrint(char* text, int x, int y);

// void lcdPrintFloat(float value, int precision, int x, int y);

// void lcdClear();

#endif //LCDMANAGER_H
