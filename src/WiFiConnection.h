#ifndef WIFICONNECTION_H
#define WIFICONNECTION_H

#include <Arduino.h>
#include <WiFi.h>
#include "SerialLogger.h"
#include "WiFiConfigOffline.h"
// #include "AzIoTConfigOffline.h"

#define LED_RED 25

static const char* ssid = IOT_CONFIG_WIFI_SSID;
static const char* password = IOT_CONFIG_WIFI_PASSWORD;

void connectToWiFi();

#endif //WIFICONNECTION_H