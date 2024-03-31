
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET 4         // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

long TimeUpdate;

void initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    //for (;;)
    //;  // Don't proceed, loop forever
  }
  // очистка буфера
  display.clearDisplay();
}

void newData(int rpm, float TINj, int T_, int T_C) {
  if (millis() - TimeUpdate >= 800) {
    TimeUpdate = millis();
    display.clearDisplay();
    display.setTextSize(1);               // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);  // Draw white text
    display.setCursor(0, 0);              // Start at top-left corner
    display.print(F("RPM :"));
    display.setTextSize(2);
    display.println(rpm);
    display.setTextSize(2);  // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.print(F("TINj:"));
    display.setTextSize(2);
    display.println(TINj);
    display.setTextSize(1);
    display.print(F("T_% :"));
    display.setTextSize(2);
    display.println(T_);
    display.setTextSize(1);
    display.print(F("T_C :"));
    display.setTextSize(2);
    display.println(T_C);
    display.display();
  }
}