#include "RelayManager.h"

bool relayState = false;

void RelayManager::init() {
  relayStateMem = flashManager.read(RELAY_PATH);
  if(relayStateMem == "1") {
    relayState = true;
    digitalWrite(RELAY, HIGH);
  } else {
    relayState = false;
    digitalWrite(RELAY, LOW);
  }
}

void RelayManager::open() {
  relayState = false;
  digitalWrite(RELAY, LOW);
  flashManager.write(RELAY_PATH, "0");
}

void RelayManager::close() {
  relayState = true;
  digitalWrite(RELAY, HIGH);
  flashManager.write(RELAY_PATH, "1");
}

bool RelayManager::toggle(char status) {

  if(status == 48) {
    open();
    return true;
  }

  if(status == 49) {
    close();
    return true;
  }

  return false;
}

RelayManager relayManager;