#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SerialLogger.h"
#include "FlashManager.h"
#include "LcdManager.h"

#define AP_SSID "x8YcF6"
#define PARAM_SSID "ssid"
#define PARAM_PASS "pass"

class WiFiManager {
  private:
    char apPass[11] = "0123456789";
    unsigned long previousMillis;
    const long interval = 10000;
    bool apActive;
    void randomizePassword(char* password, int length);
  public:
    WiFiManager();
    bool init();
    void setAP();
    bool isAPActive();
};

extern WiFiManager wifiManager;
extern String ssid;
extern String pass;

#endif //WIFIMANAGER_H
