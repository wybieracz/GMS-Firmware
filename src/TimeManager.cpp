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
  utc = 1;
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

char* TimeManager::getDataString() {
  refresh();

  itoa(dataTime.tm_year + UNIX_EPOCH_START_YEAR, dateString, 10);
  dateString[4] = '-';

  if(dataTime.tm_mon < 10) {
    dateString[5] = '0';
    itoa(dataTime.tm_mon, dateString + 6, 10);
  } else itoa(dataTime.tm_mon, dateString + 5, 10);
  dateString[7] = '-';

  if(dataTime.tm_mday < 10) {
    dateString[8] = '0';
    itoa(dataTime.tm_mday, dateString + 9, 10);
  } else itoa(dataTime.tm_mday, dateString + 8, 10);
  dateString[10] = ' ';

  if(dataTime.tm_hour < 10) {
    dateString[11] = '0';
    itoa(dataTime.tm_hour, dateString + 12, 10);
  } else itoa(dataTime.tm_hour + utc, dateString + 11, 10);
  dateString[13] = ':';

  if(dataTime.tm_min < 10) {
    dateString[14] = '0';
    itoa(dataTime.tm_min, dateString + 15, 10);
  } else itoa(dataTime.tm_min, dateString + 14, 10);
  dateString[16] = '\0';

  return dateString;
}

TimeManager timeManager;
