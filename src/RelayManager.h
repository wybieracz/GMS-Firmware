#ifndef RELAYMANAGER_H
#define RELAYMANAGER_H

//#include <Arduino.h>
#include "PinManager.h"
#include "FlashManager.h"
#include "TimeManager.h"

class RelayManager {
  private:
    String modeMem;
    String rulesMem;
    String stateMem;
    long int limDate;
    void close();
    void open();
    void checkRulesTimeMode();
    void checkRulesAutoMode();
  public:
    bool state;
    short int mode;
    char rules[200];
    void init();
    bool toggle(char status);
    bool setMode(char* data);
    void checkRules();
};

extern RelayManager relayManager;

#endif //RELAYMANAGER_H