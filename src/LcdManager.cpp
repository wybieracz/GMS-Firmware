#include "LcdManager.h"

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

void LcdManager::init() {
  lcd.begin(16, 2);
  ledcSetup(LED_CH, FREQ, LED_RES);
  ledcAttachPin(LED_LCD, LED_CH);

  lcdState = flashManager.read(LCD_PATH);
  if(lcdState=="") lcdState = "255";

  (void)setBrightness(atoi(lcdState.c_str()));
  lcd.setCursor(0, 0);
}

bool LcdManager::setBrightness(int brightness) {
  if(brightness >= 0 && brightness <= 255) {
    char brightnessString[4];
    flashManager.write(LCD_PATH, itoa(brightness, brightnessString, 10));
    ledcWrite(LED_CH, brightness);
    return true;
  } else return false;
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
