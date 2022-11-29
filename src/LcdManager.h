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
    String settingsMem;
  public:
    int brightness;
    char settings[3];
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

#endif //LCDMANAGER_H
