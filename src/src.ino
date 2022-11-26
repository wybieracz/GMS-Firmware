#include <Arduino.h>
#include "TimeManager.h"
#include "WiFiManager.h"
#include "AzIoTClient.h"
#include "EnergyManager.h"

void setup() {

  initPinout();
  flashManager.init();
  lcdManager.init();
  relayManager.init();

  if(wifiManager.init()) {
    timeManager.init();
    // initIoTHubClient();
    // (void)initMqttClient();
    iotClient.init();
  } else {
    wifiManager.setAP();
  }
}

void loop() {
  if (!wifiManager.isAPActive()) {
  if (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_RED, LOW);
      if(!wifiManager.init()) wifiManager.setAP();
    }
    else {
      iotClient.check();
      // azIoTClientLoop();
      energyManager.calc(20, 1000);
      energyManager.print();
    }
  } 
}
