#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>

#include "TouchScreen.h"

#define BLACK 0x0000
#define RED 0xF800
#define WHITE 0xFFFF
MCUFRIEND_kbv tft;

// These are the pins for the shield!
// #define YP A1  // must be an analog pin, use "An" notation!
// #define XM A2  // must be an analog pin, use "An" notation!
// #define YM 7   // can be a digital pin
// #define XP 6   // can be a digital pin
#define YP A2  
#define XM A3 
#define YM 8   
#define XP 9   
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

void setup() {
  uint16_t ID = tft.readID();
  tft.begin(ID);
  
  tft.fillScreen(BLACK);
  tft.setCursor(0,50);
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.print("my first project with tft -");
  Serial.begin(9600);

  tft.fillTriangle(40, 100, 40, 200, 140, 150, tft.color565(0, 0, 255));

}

void loop() {
  // a point object holds x y and z coordinates
  TSPoint p = ts.getPoint();
  
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
     Serial.print("X = "); Serial.print(p.x);
     Serial.print("\tY = "); Serial.print(p.y);
     Serial.print("\tPressure = "); Serial.println(p.z);
  }
}