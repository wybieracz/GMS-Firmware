#include "RelayManager.h"

void RelayManager::init() {
  stateMem = flashManager.read(RELAY_PATH);
  if(stateMem == "1") {
    state = true;
    digitalWrite(RELAY, HIGH);
  } else {
    state = false;
    digitalWrite(RELAY, LOW);
  }

  modeMem = flashManager.read(MODE_PATH);
  if(modeMem == "2") {
    mode = 2;
  } else if(modeMem == "3") {
    mode = 3;
  } else {
    mode = 1;
    flashManager.write(MODE_PATH, "1");
  }
  
  rulesMem = flashManager.read(RULES_PATH);
  strcpy(rules, rulesMem.c_str());
  if(mode == 2) limDate = atoi(rules + 1);
}

void RelayManager::open() {
  state = false;
  digitalWrite(RELAY, LOW);
  flashManager.write(RELAY_PATH, "0");
}

void RelayManager::close() {
  state = true;
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

bool RelayManager::setMode(char* data) {
  if(data[0] == '3') {
    strcpy(rules, data + 1);
    flashManager.write(RULES_PATH, rules);
    mode = 3;
    flashManager.write(MODE_PATH, "3");
    return true;
  } else if(data[0] == '2') {
    strcpy(rules, data + 1);
    flashManager.write(RULES_PATH, rules);
    limDate = atoi(rules + 1);
    mode = 2;
    flashManager.write(MODE_PATH, "2");
    return true;
  } else {
    mode = 1;
    flashManager.write(MODE_PATH, "1");
    toggle(data[1]);
    return true;
  }
  return false;
}

void RelayManager::checkRulesTimeMode() {
  time_t now = time(NULL);
  Serial.println("rules");
  Serial.println(rules + 1);
  Serial.println(now);
  Serial.println(limDate);
  if(now > limDate) {
    if(rules[0] == 48 && state) {
      state = false;
      digitalWrite(RELAY, LOW);
      flashManager.write(RELAY_PATH, "0");
    }
    if(rules[0] == 49 && !state) {
      state = true;
      digitalWrite(RELAY, HIGH);
      flashManager.write(RELAY_PATH, "1");
    }
  } else {
    if(rules[0] == 49 && state) {
      state = false;
      digitalWrite(RELAY, LOW);
      flashManager.write(RELAY_PATH, "0");
    }
    if(rules[0] == 48 && !state) {
      state = true;
      digitalWrite(RELAY, HIGH);
      flashManager.write(RELAY_PATH, "1");
    }
  }
}

void RelayManager::checkRulesAutoMode() {
  char buffer[] = "00";
  char* ptr = rules;
  bool newRelayState = false;
  int day, hStart, mStart, hEnd, mEnd;
  timeManager.refresh(false);

  while(*ptr) {
    buffer[0] = '0';
    buffer[1] = *ptr;
    day = atoi(buffer);
    if(day == timeManager.get.tm_wday) {
      ptr++;
      //hStart
      buffer[0] = *ptr;
      ptr++;
      buffer[1] = *ptr;
      ptr++;
      hStart = atoi(buffer);

      //mStart
      buffer[0] = *ptr;
      ptr++;
      buffer[1] = *ptr;
      ptr++;
      mStart = atoi(buffer);

      //hEnd
      buffer[0] = *ptr;
      ptr++;
      buffer[1] = *ptr;
      ptr++;
      hEnd = atoi(buffer);

      //mEnd
      buffer[0] = *ptr;
      ptr++;
      buffer[1] = *ptr;
      ptr++;
      mEnd = atoi(buffer);

      if((timeManager.get.tm_hour == hStart && timeManager.get.tm_hour == hEnd &&
        timeManager.get.tm_min >= mStart && timeManager.get.tm_min < mEnd) ||
        (timeManager.get.tm_hour > hStart && timeManager.get.tm_hour < hEnd) ||
        (timeManager.get.tm_hour == hStart && timeManager.get.tm_hour != hEnd && timeManager.get.tm_min >= mStart) ||
        (timeManager.get.tm_hour == hEnd && timeManager.get.tm_hour != hStart && timeManager.get.tm_min < mEnd)) {
          newRelayState = true;
          break;
      }
    } else ptr = ptr + 9;
  }

  if(state != newRelayState) {
    if(newRelayState) {
      state = true;
      digitalWrite(RELAY, HIGH);
      flashManager.write(RELAY_PATH, "1");
    } else {
      state = false;
      digitalWrite(RELAY, LOW);
      flashManager.write(RELAY_PATH, "0");
    }
  }
}

void RelayManager::checkRules() {
  if(mode == 2) checkRulesTimeMode();
  else if(mode == 3) checkRulesAutoMode();
}

RelayManager relayManager;