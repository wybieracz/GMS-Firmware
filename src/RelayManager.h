#ifndef RELAYMANAGER_H
#define RELAYMANAGER_H

//#include <Arduino.h>
#include "PinManager.h"
#include "FlashManager.h"
#include "TimeManager.h"

class RelayManager {
  private:
    short int mode;
    String modeMem;
    bool relayState;
    String relayStateMem;
    char rules[200];
    String rulesMem;
    long int limDate;
    void close();
    void open();
    void checkRulesTimeMode();
    void checkRulesAutoMode();
  public:
    void init();
    bool toggle(char status);
    bool getRelayState();
    bool setMode(char* data);
    void checkRules();
};

extern RelayManager relayManager;

#endif //RELAYMANAGER_H