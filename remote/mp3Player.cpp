#include <Arduino.h>

#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>


#define MP3_RX_PIN 52
#define MP3_TX_PIN 53

SoftwareSerial mp3Serial(52, 53);
DFRobotDFPlayerMini mp3Player;


// These are the pins for the shield!
#define YP A2
#define XM A3
#define YM 8
#define XP 9
#define BLACK 0x0000
#define RED 0xF800
#define WHITE 0xFFFF
MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

void drawPreviousButton(int x, int y, int width, int height, uint16_t color);
void drawNextButton(int x, int y, int width, int height, uint16_t color);
void drawPlayButton(int x, int y, int width, int height, uint16_t color);
void drawPauseButton(int x, int y, int width, int height, uint16_t color);

#define BUTTON_WIDTH 80
#define BUTTON_HEIGHT 80
#define SCREEN_WIDTH 320
#define BUTTON_Y 100
#define BUTTON_GAP ((SCREEN_WIDTH - BUTTON_WIDTH * 3) / 4)


void setupScreen() {
  uint16_t ID = tft.readID();
  tft.begin(ID);

  tft.fillScreen(BLACK);
  tft.setCursor(0, 50);
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.print("my first project with tft -");
  drawPreviousButton(BUTTON_GAP, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, WHITE);
  drawPlayButton(BUTTON_WIDTH + 2 * BUTTON_GAP, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, WHITE);
  drawPauseButton(BUTTON_WIDTH + 2 * BUTTON_GAP, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, WHITE);
  drawNextButton(BUTTON_WIDTH * 2 + 3 * BUTTON_GAP, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, WHITE);
}

void setupMp3Player() {
  mp3Serial.begin(9600);

  Serial.println();
  Serial.println("DFRobot DFPlayer Mini Demo");
  Serial.println("Initializing DFPlayer ... (May take 3~5 seconds)");

  if (!mp3Player.begin(mp3Serial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println("Unable to begin:");
    Serial.println("1.Please recheck the connection!");
    Serial.println("2.Please insert the SD card!");
    while (true) {
      delay(0);  // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println("DFPlayer Mini online.");

  mp3Player.volume(30);  //Set volume value. From 0 to 30

}

void drawPlayButton(int x, int y, int width, int height, uint16_t color) {
  tft.fillRect(x, y, width, height, BLACK);
  tft.drawRect(x, y, width, height, color);
  tft.fillTriangle(x + width * 19 / 100, y + 2, x + width * 19 / 100, y + height - 2, x + width * 81 / 100, y + height / 2, color);
}

void drawPauseButton(int x, int y, int width, int height, uint16_t color) {
  tft.fillRect(x, y, width, height, BLACK);
  tft.drawRect(x, y, width, height, color);
  tft.fillRect(x + width * 12 / 100, y + 2, width * 28 / 100, height - 2, color);
  tft.fillRect(x + width * 60 / 100, y + 2, width * 28 / 100, height - 2, color);
}

void drawPreviousButton(int x, int y, int width, int height, uint16_t color) {
  tft.drawRect(x, y, width, height, color);

  tft.fillRect(x + width * 12 / 100, y + 2, width * 28 / 100, height - 2, color);
  tft.fillTriangle(x + width * 88 / 100, y + 2, x + width * 88 / 100, y + height - 2, x + width * 26 / 100, y + height / 2, color);
}

void drawNextButton(int x, int y, int width, int height, uint16_t color) {
  tft.drawRect(x, y, width, height, color);

  tft.fillRect(x + width * 60 / 100, y + 2, width * 28 / 100, height - 2, color);
  tft.fillTriangle(x + width * 12 / 100, y + 2, x + width * 12 / 100, y + height - 2, x + width * 74 / 100, y + height / 2, color);
}