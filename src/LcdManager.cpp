#include "LcdManager.h"

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

LcdManager::LcdManager() {
  lcd.begin(16, 2);
  ledcSetup(LED_CH, FREQ, LED_RES);
  ledcAttachPin(LED_LCD, LED_CH);
  lcd.setCursor(0, 0);
}

void LcdManager::init() {
  
  brightnessMem = flashManager.read(LCD_BRIGHTNESS_PATH);
  if(brightnessMem=="") brightnessMem = "255";
  ledcWrite(LED_CH, atoi(brightnessMem.c_str()));

  settingsMem = flashManager.read(LCD_SETTINGS_PATH);
  if(settingsMem=="") settingsMem = "12";
  strcpy(settings, settingsMem.c_str());
}

bool LcdManager::setBrightness(char* data) {

  int temp = atoi(data);

  if(temp < 0 && temp > 255) return false;
  brightness = temp;
  ledcWrite(LED_CH, brightness);
  flashManager.write(LCD_BRIGHTNESS_PATH, data);
  return true;
}

bool LcdManager::setDisplay(char* data) {

  if(data[0] < 49 || data[0] > 52 || data[0] < 49 || data[1] > 52) return false;
  settings[0] = data[0];
  settings[1] = data[1];
  settings[3] = '\0';
  flashManager.write(LCD_SETTINGS_PATH, settings);
  return true;
}

char* LcdManager::getSettings() {
  return settings;
}

void LcdManager::print(char* text, int x, int y) {
  lcd.setCursor(x, y);
  lcd.print(text);
}

void LcdManager::printFloat(float value, int precision, int x, int y) {
  if(value < 300.0) {
    lcd.setCursor(x, y);
    lcd.print(value, precision);
  }
}

void LcdManager::clear() {
  lcd.clear();
}

LcdManager lcdManager;
