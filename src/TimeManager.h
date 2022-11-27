#ifndef TIME_H
#define TIME_H

#include <Arduino.h>
#include <time.h>
#include "SerialLogger.h"

#define UNIX_TIME_NOV_13_2017 1510592825
#define UNIX_EPOCH_START_YEAR 1900
#define NTP_SERVERS "0.pl.pool.ntp.org", "1.pl.pool.ntp.org"

#define PST_TIME_ZONE 2
#define PST_TIME_ZONE_DAYLIGHT_SAVINGS_DIFF 1

#define GMT_OFFSET_SECS (PST_TIME_ZONE * 3600)
#define GMT_OFFSET_SECS_DST ((PST_TIME_ZONE + PST_TIME_ZONE_DAYLIGHT_SAVINGS_DIFF) * 3600)

class TimeManager {
  private:
    short utc;
    char dateString[20];
  public:
    struct tm get;
    void refresh(bool local);
    void init();
    char* getDataString(bool local, bool seconds);
};

extern TimeManager timeManager;

#endif //TIME_H
