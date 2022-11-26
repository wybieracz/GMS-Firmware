#include "TimeManager.h"

void TimeManager::refresh() {
  time_t now = time(NULL);
  gmtime_r(&now, &dataTime);
}

void TimeManager::init() {

  logger.info("Setting time using SNTP");

  configTime(GMT_OFFSET_SECS, GMT_OFFSET_SECS_DST, NTP_SERVERS);
  time_t now = time(NULL);
  
  while (now < UNIX_TIME_NOV_13_2017) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  
  Serial.println("");
  gmtime_r(&now, &dataTime);
  logger.info("Time initialized!");
}

int TimeManager::getYear() {
  refresh();
  return dataTime.tm_year + UNIX_TIME_NOV_13_2017;
}

int TimeManager::getMonth() {
  refresh();
  return dataTime.tm_mon;
}

int TimeManager::getDay() {
  refresh();
  return dataTime.tm_mday;
}

int TimeManager::getHour() {
  refresh();
  return dataTime.tm_hour;
}

int TimeManager::getMinutes() {
  refresh();
  return dataTime.tm_min;
}

int TimeManager::getSeconds() {
  refresh();
  return dataTime.tm_sec;
}

TimeManager timeManager;