#include "TimeManager.h"

void TimeManager::refresh(bool local) {
  time_t now = time(NULL);
  if(local) now = now + 3600 * utc;
  gmtime_r(&now, &get);
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
  gmtime_r(&now, &get);
  utc = 1;
  logger.info("Time initialized!");
}

char* TimeManager::getDataString(bool local, bool seconds) {
  refresh(local);

  itoa(get.tm_year + UNIX_EPOCH_START_YEAR, dateString, 10);
  dateString[4] = '-';

  if(get.tm_mon + 1 < 10) {
    dateString[5] = '0';
    itoa(get.tm_mon + 1, dateString + 6, 10);
  } else itoa(get.tm_mon + 1, dateString + 5, 10);
  dateString[7] = '-';

  if(get.tm_mday < 10) {
    dateString[8] = '0';
    itoa(get.tm_mday, dateString + 9, 10);
  } else itoa(get.tm_mday, dateString + 8, 10);
  dateString[10] = ' ';

  if(get.tm_hour < 10) {
    dateString[11] = '0';
    itoa(get.tm_hour, dateString + 12, 10);
  } else itoa(get.tm_hour, dateString + 11, 10);
  dateString[13] = ':';

  if(get.tm_min < 10) {
    dateString[14] = '0';
    itoa(get.tm_min, dateString + 15, 10);
  } else itoa(get.tm_min, dateString + 14, 10);
  dateString[16] = ':';

  if(get.tm_sec < 10) {
    dateString[17] = '0';
    itoa(get.tm_sec, dateString + 18, 10);
  } else itoa(get.tm_sec, dateString + 17, 10);
  dateString[19] = '\0';

  if(!seconds) dateString[16] = '\0';

  return dateString;
}

TimeManager timeManager;
