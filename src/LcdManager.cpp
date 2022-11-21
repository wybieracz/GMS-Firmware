#include "LcdManager.h"

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

void initLCD() {
  lcd.begin(16, 2);
  ledcSetup(LED_CH, FREQ, LED_RES);
  ledcAttachPin(LED_LCD, LED_CH);

  lcdState = readFile(SPIFFS, lcdStatePath);
  if(lcdState=="") lcdState = "255";

  (void)setLcdBrightness(atoi(lcdState.c_str()));
  lcd.setCursor(0, 0);
  lcd.print("GM System");
}

bool setLcdBrightness(int brightness) {
  if(brightness >= 0 && brightness <= 255) {
    char brightnessString[4];
    writeFile(SPIFFS, lcdStatePath, itoa(brightness, brightnessString, 10));
    ledcWrite(LED_CH, brightness);
    return true;
  } else return false;
}
