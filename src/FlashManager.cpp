#include "FlashManager.h"

String ssid;
String pass;
String relayStateMem;
String lcdState;
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* relayStatePath = "/relayState.txt";
const char* lcdStatePath = "/lcdState.txt";

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Logger.Error("An error has occurred while mounting SPIFFS.");
  }
  Logger.Info("SPIFFS mounted successfully.");
}

// Read File from SPIFFS
String readFile(fs::FS &fs, const char * path) {
  Logger.Info("Reading file: " + String(path));

  File file = fs.open(path);
  if(!file || file.isDirectory()){
    Logger.Error("Failed to open file for reading.");
    return String();
  }
  
  String fileContent;
  while(file.available()) {
    fileContent = file.readStringUntil('\n');
    break;
  }
  return fileContent;
}

// Write file to SPIFFS
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Logger.Info("Writing file: " + String(path));

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Logger.Error("Failed to open file for writing.");
    return;
  }
  if(file.print(message)) {
    Logger.Info("File written.");
  } else {
    Logger.Error("Write failed.");
  }
}
