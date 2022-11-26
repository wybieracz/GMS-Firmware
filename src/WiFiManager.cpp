#include "WiFiManager.h"

AsyncWebServer server(80);
String ssid;
String pass;

WiFiManager::WiFiManager() {
  previousMillis = 0;
  apActive = false;
}

void WiFiManager::randomizePassword(char* password, int length) {
  int temp = 48;
  for(int i = 0; i < length; i++) {
    temp = 33 + (esp_random() % 90);
    if(temp == 39 || temp == 96) temp++;
    password[i] = temp;
  }
}

bool WiFiManager::init() {
  // Load values saved in flash
  ssid = flashManager.read(SSID_PATH);
  pass = flashManager.read(PASS_PATH);

  if(ssid=="") {
    logger.error("Undefined SSID or IP address.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
  logger.info("Connecting to WiFi");
  lcdManager.clear();
  
  lcdManager.print("Connecting to", 0, 0);
  
  lcdManager.print("WiFi", 0, 1);

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;
  short int i = 5;

  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    
    lcdManager.print(".", i, 1);
    currentMillis = millis();
    i++;
    if(i > 15) i = 15;
    if (currentMillis - previousMillis >= interval) {
      logger.error("Failed to connect.");
      lcdManager.clear();
      
      lcdManager.print("WiFi failed!", 0, 0);
      
      lcdManager.print("Setting AP", 0, 1);
      return false;
    }
  }

  apActive = false;
  Serial.println(WiFi.localIP());
  digitalWrite(LED_RED, HIGH);
  return true;
}

void WiFiManager::setAP() {
  // Connect to Wi-Fi network with SSID and password
  apActive = true;
  digitalWrite(LED_BLUE, HIGH);
  randomizePassword(apPass, 10);
  logger.info("Setting AP (Access Point)");
  logger.info("SSID: " + String(AP_SSID));
  logger.info("PASS: " + String(apPass));
  lcdManager.clear();
  
  lcdManager.print("SSID: ", 0, 0);
  
  lcdManager.print(AP_SSID, 6, 0);
  
  lcdManager.print("PASS: ", 0, 1);
  
  lcdManager.print(apPass, 6, 1);
  WiFi.softAP(AP_SSID, apPass);
  delay(100);
  IPAddress IP = WiFi.softAPIP();
  logger.info("AP IP address: " + String(IP));

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
        if (p->name() == PARAM_SSID) {
          ssid = p->value().c_str();
          logger.info("SSID set to: " + String(ssid));
          // Write file to save value
          flashManager.write(SSID_PATH, ssid.c_str());
        }
        // HTTP POST pass value
        if (p->name() == PARAM_PASS) {
          pass = p->value().c_str();
          logger.info("Password set to: " + String(pass));
          // Write file to save value
          flashManager.write(PASS_PATH, pass.c_str());
        }
      }
    }
    request->send(200, "text/plain", "Success! The device will restart and connect to your WiFi.");

    lcdManager.clear();
    lcdManager.print("Rebooting", 0, 0);
    
    for(int i = 9; i < 12; i++) {
      lcdManager.print(".", i, 0);
      delay(500);
    }
    delay(500);
    digitalWrite(LED_BLUE, LOW);
    ESP.restart();
  });
  server.begin();
}

bool WiFiManager::isAPActive() {
  return apActive;
}

WiFiManager wifiManager;
