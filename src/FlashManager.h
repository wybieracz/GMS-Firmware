#ifndef FLASHMANAGER_H
#define FLASHMANAGER_H

#include <string> 
#include "PinManager.h"
#include "SerialLogger.h"
#include "SPIFFS.h"

#define SSID_PATH "/ssid.txt"
#define PASS_PATH "/pass.txt"
#define RELAY_PATH "/relay.txt"
#define LCD_BRIGHTNESS_PATH "/lcdBrightness.txt"
#define LCD_SETTINGS_PATH "/lcdSettings.txt"
#define KWH_PATH "/kWh.txt"
#define RESET_PATH "/reset.txt"
#define PERIOD_PATH "/period.txt"
#define LAST_DAY_PATH "/day.txt"
#define MODE_PATH "/mode.txt"
#define RULES_PATH "/rules.txt"

class FlashManager {
  public:
    void init();
    String read(const char * path);
    void write(const char * path, const char * message);
};

extern FlashManager flashManager;

#endif //FLASHMANAGER_H
