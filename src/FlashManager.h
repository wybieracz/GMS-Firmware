#ifndef FLASHMANAGER_H
#define FLASHMANAGER_H

#include <string> 
#include "PinManager.h"
#include "SerialLogger.h"
#include "SPIFFS.h"

//Variables to save values from HTML form
extern String ssid;
extern String pass;
extern String relayStateMem;
extern String lcdState;

// File paths to save input values permanently
extern const char* ssidPath;
extern const char* passPath;
extern const char* relayStatePath;
extern const char* lcdStatePath;

void initSPIFFS();

String readFile(fs::FS &fs, const char * path);

void writeFile(fs::FS &fs, const char * path, const char * message);

#endif //FLASHMANAGER_H
