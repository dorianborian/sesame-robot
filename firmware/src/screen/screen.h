#pragma once
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define OLED_I2C_ADDR  0x3C

extern Adafruit_SSD1306 display;

void initScreen();

void showSetupMessage(const String& msg);
void updateFaceBitmap(const unsigned char* bitmap);
void drawWifiInfoScroll(const String& text, int scrollPos, const unsigned char* faceBitmap);
