#include "FlashManager.h"

// Initialize SPIFFS
void FlashManager::init() {
  if (!SPIFFS.begin(true)) {
    logger.error("An error has occurred while mounting SPIFFS.");
  }
  logger.info("SPIFFS mounted successfully.");
}

// Read File from SPIFFS
String FlashManager::read(const char * path) {
  logger.info("Reading file: " + String(path));

  File file = SPIFFS.open(path);
  if(!file || file.isDirectory()){
    logger.error("Failed to open file for reading.");
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
void FlashManager::write(const char * path, const char * message) {
  logger.info("Writing file: " + String(path));

  File file = SPIFFS.open(path, FILE_WRITE);
  if(!file) {
    logger.error("Failed to open file for writing.");
    return;
  }
  if(file.print(message)) {
    logger.info("File written.");
  } else {
    logger.error("Write failed.");
  }
}

FlashManager flashManager;
