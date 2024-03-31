float Rpm;
#include "Display.h"



void setup() {
  Serial.begin(9600);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  initDisplay();
  
}
void loop() {
  newData(Rpm, 3.14500, 56, 111);  // Draw 'stylized' characters
  if (Rpm >= 6000) {
    Rpm = 0;
  }
}