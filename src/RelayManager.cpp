#include "RelayManager.h"

bool relayState = false;

void initRelay() {
  relayStateMem = readFile(SPIFFS, relayStatePath);
  if(relayStateMem == "1") {
    relayState = true;
    digitalWrite(RELAY, HIGH);
  } else {
    relayState = false;
    digitalWrite(RELAY, LOW);
  }
}

void openRelay() {
  relayState = false;
  digitalWrite(RELAY, LOW);
  writeFile(SPIFFS, relayStatePath, "0");
}

void closeRelay() {
  relayState = true;
  digitalWrite(RELAY, HIGH);
  writeFile(SPIFFS, relayStatePath, "1");
}