#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SerialLogger.h"
#include "FlashManager.h"
#include "LcdManager.h"

#define AP_SSID "x8YcF6"

// Create AsyncWebServer object on port 80
static AsyncWebServer server(80);

// Search for parameter in HTTP POST request
static const char* PARAM_INPUT_1 = "ssid";
static const char* PARAM_INPUT_2 = "pass";
static char apPass[] = "0123456789";

static unsigned long previousMillis = 0;
static const long interval = 10000;

extern bool apActive;

void randomizePassword(char* password, int length);

bool initWiFi();

void setAP();

#endif //WIFIMANAGER_H
