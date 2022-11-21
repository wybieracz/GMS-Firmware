#include <Arduino.h>
#include "Time.h"
#include "WiFiManager.h"
#include "AzIoTClient.h"
#include "LcdManager.h"

void setup() {

  initPinout();
  initSPIFFS();
  initLCD();
  initRelay();

  if(initWiFi()) {
    initializeTime();
    initializeIoTHubClient();
    (void)initializeMqttClient();
  } else {
    setAP();
  }
  
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_RED, LOW);
    if(!initWiFi()) setAP();
  }
  else azIoTClientLoop();
}
