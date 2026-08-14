#include "screen.h"
#include <Wire.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initScreen() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
    Serial.println(F("SSD1306 allocation failed."));
    while (1);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.display();
}

void showSetupMessage(const String& msg) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(msg);
  display.display();
}

void updateFaceBitmap(const unsigned char* bitmap) {
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  display.display();
}

void drawWifiInfoScroll(const String& text, int scrollPos, const unsigned char* faceBitmap) {
  display.clearDisplay();

  if (faceBitmap != nullptr) {
    display.drawBitmap(0, 0, faceBitmap, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  }

  display.fillRect(0, 0, SCREEN_WIDTH, 10, SSD1306_BLACK);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);
  display.setCursor(-scrollPos, 1);
  display.print(text);
  display.setTextWrap(true);

  display.display();
}
