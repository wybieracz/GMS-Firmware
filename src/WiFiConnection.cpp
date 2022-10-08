#include "WiFiConnection.h"

void connectToWiFi() {

  Logger.Info("Connecting to WIFI SSID: " + String(ssid));

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(750);
    Serial.print(".");
  }

  digitalWrite(LED_RED, HIGH);
  Serial.println("");
  Logger.Info("WiFi connected, IP address: " + WiFi.localIP().toString());
}