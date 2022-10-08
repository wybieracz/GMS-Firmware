#include "Time.h"
#include "WiFiConnection.h"
#include "AzIoTClient.h"

void setup() {

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  
  connectToWiFi();
  initializeTime();
  initializeIoTHubClient();
  (void)initializeMqttClient();
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_RED, LOW);
    connectToWiFi();
  }
  else azIoTClientLoop();
}
