#include <Arduino.h>
#include "TimeManager.h"
#include "WiFiManager.h"
#include "AzIoTClient.h"
#include "EnergyManager.h"

void setup() {
  delay(2000);
  initPinout();
  flashManager.init();
  lcdManager.init();
  relayManager.init();
  energyManager.init();

  if(wifiManager.init()) {
    timeManager.init();
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
      energyManager.calc(20, 1000);
      energyManager.print();
      energyManager.display();
    }
  } 
}
