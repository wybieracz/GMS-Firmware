#ifndef FLASHMANAGER_H
#define FLASHMANAGER_H

#include <string> 
#include "PinManager.h"
#include "SerialLogger.h"
#include "SPIFFS.h"

#define SSID_PATH "/ssid.txt"
#define PASS_PATH "/pass.txt"
#define RELAY_PATH "/relay.txt"
#define LCD_PATH "/lcd.txt"

class FlashManager {
  //Variables to save values from HTML form
  // String ssid;
  // String pass;
  // String relayStateMem;
  // String lcdState;

  // File paths to save input values permanently
  // const char* ssidPath;
  // const char* passPath;
  // const char* relayStatePath;
  // const char* lcdStatePath;
  public:
    void init();
    String read(const char * path);
    void write(const char * path, const char * message);
};

extern FlashManager flashManager;

//Variables to save values from HTML form
// extern String ssid;
// extern String pass;
// extern String relayStateMem;
// extern String lcdState;

// File paths to save input values permanently
// extern const char* ssidPath;
// extern const char* passPath;
// extern const char* relayStatePath;
// extern const char* lcdStatePath;

// void initSPIFFS();

// String readFile(fs::FS &fs, const char * path);

// void writeFile(fs::FS &fs, const char * path, const char * message);

#endif //FLASHMANAGER_H
