#include "WiFiManager.h"

void randomizePassword(char* password, int length) {
  for(int i = 0; i < length; i++) {
    password[i] = 33 + (esp_random() % 90);
  }
}

bool initWiFi() {

  // Load values saved in SPIFFS
  ssid = readFile(SPIFFS, ssidPath);
  pass = readFile(SPIFFS, passPath);

  if(ssid=="") {
    Logger.Error("Undefined SSID or IP address.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
  Logger.Info("Connecting to WiFi");

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while(WiFi.status() != WL_CONNECTED) {
    delay(750);
    Serial.print(".");
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Logger.Error("Failed to connect.");
      return false;
    }
  }

  Serial.println(WiFi.localIP());
  digitalWrite(LED_RED, HIGH);
  return true;
}

void setAP() {
  // Connect to Wi-Fi network with SSID and password
  digitalWrite(LED_BLUE, HIGH);
  Logger.Info("Setting AP (Access Point)");
  randomizePassword(apPass, 10);
  Logger.Info("SSID: " + String(AP_SSID));
  Logger.Info("PASS: " + String(apPass));
  WiFi.softAP(AP_SSID, apPass);

  IPAddress IP = WiFi.softAPIP();
  Logger.Info("AP IP address: " + String(IP));

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  
  server.serveStatic("/", SPIFFS, "/");
  
  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost()){
        // HTTP POST ssid value
        if (p->name() == PARAM_INPUT_1) {
          ssid = p->value().c_str();
          Logger.Info("SSID set to: " + String(ssid));
          // Write file to save value
          writeFile(SPIFFS, ssidPath, ssid.c_str());
        }
        // HTTP POST pass value
        if (p->name() == PARAM_INPUT_2) {
          pass = p->value().c_str();
          Logger.Info("Password set to: " + String(pass));
          // Write file to save value
          writeFile(SPIFFS, passPath, pass.c_str());
        }
      }
    }
    request->send(200, "text/plain", "Success! The device will restart and connect to your WiFi.");
    delay(2000);
    digitalWrite(LED_BLUE, LOW);
    ESP.restart();
  });
  server.begin();
}