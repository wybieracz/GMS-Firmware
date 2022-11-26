#ifndef RELAYMANAGER_H
#define RELAYMANAGER_H

//#include <Arduino.h>
#include "PinManager.h"
#include "FlashManager.h"

class RelayManager {
  private:
    bool relayState;
    String relayStateMem;
  public:
    void init();
    void open();
    void close();
};

extern RelayManager relayManager;

#endif //RELAYMANAGER_H