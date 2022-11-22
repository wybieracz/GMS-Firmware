#include "WiFiManager.h"

bool apActive = false;

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
  lcdClear();
  lcdPrint("Connecting to", 0, 0);
  lcdPrint("WiFi", 0, 1);

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;
  short int i = 5;

  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    lcdPrint(".", i, 1);
    currentMillis = millis();
    i++;
    if(i > 15) i = 15;
    if (currentMillis - previousMillis >= interval) {
      Logger.Error("Failed to connect.");
      lcdClear();
      lcdPrint("WiFi failed!", 0, 0);
      lcdPrint("Setting AP", 0, 1);
      return false;
    }
  }

  apActive = false;
  Serial.println(WiFi.localIP());
  digitalWrite(LED_RED, HIGH);
  return true;
}

void setAP() {
  // Connect to Wi-Fi network with SSID and password
  apActive = true;
  digitalWrite(LED_BLUE, HIGH);
  randomizePassword(apPass, 10);
  Logger.Info("Setting AP (Access Point)");
  Logger.Info("SSID: " + String(AP_SSID));
  Logger.Info("PASS: " + String(apPass));
  lcdClear();
  lcdPrint("SSID: ", 0, 0);
  lcdPrint(AP_SSID, 6, 0);
  lcdPrint("PASS: ", 0, 1);
  lcdPrint(apPass, 6, 1);
  WiFi.softAP(AP_SSID, apPass);
  delay(100);
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
    lcdClear();
    lcdPrint("Rebooting", 0, 0);
    
    for(int i = 9; i < 12; i++) {
      lcdPrint(".", i, 0);
      delay(500);
    }
    delay(500);
    digitalWrite(LED_BLUE, LOW);
    ESP.restart();
  });
  server.begin();
}
