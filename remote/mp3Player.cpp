#include <Arduino.h>

#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>

#include "common.h"
#include "mp3Player.h"

#define MP3_RX_PIN 52
#define MP3_TX_PIN 53

SoftwareSerial mp3Serial(52, 53);
DFRobotDFPlayerMini mp3Player;

#define NUMBER_OF_SONGS 3
#define SONG_NAME_MAX_LENGTH 18
#define ARTIST_MAX_LENGTH 9

char songNames[NUMBER_OF_SONGS][SONG_NAME_MAX_LENGTH] = {
    "El leila",
    "Amaken el sahar",
    "Wayah",
};

char artistNames[NUMBER_OF_SONGS][ARTIST_MAX_LENGTH] = {
    "Amr Diab",
    "Amr Diab",
    "Amr Diab",
};
volatile int currentSongNumber = 0;
volatile bool isPlaying = false;
volatile bool isPaused = false;

bool pauseDisplayed = false;
int displayedSong = -1;

// These are the pins for the shield!
#define YP A2
#define XM A3
#define YM 8
#define XP 9
#define BLACK 0x0000
#define RED 0xF800
#define WHITE 0xFFFF
MCUFRIEND_kbv tft;

#define MINPRESSURE 100
#define MAXPRESSURE 1000
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

void drawPreviousButton(int x, int y, int width, int height, uint16_t color);
void drawNextButton(int x, int y, int width, int height, uint16_t color);
void drawPlayButton(int x, int y, int width, int height, uint16_t color);
void drawPauseButton(int x, int y, int width, int height, uint16_t color);

const int32_t BUTTON_WIDTH = 80;
const int32_t BUTTON_HEIGHT = 80;
const int32_t SCREEN_WIDTH = 320;
const int32_t SCREEN_HEIGHT = 480;
const int32_t BUTTON_Y = 100;
const int32_t BUTTON_GAP = ((SCREEN_WIDTH - BUTTON_WIDTH * 3) / 4);

const int32_t TOUCH_LEFT_X = 150;
const int32_t TOUCH_RIGHT_X = 900;
const int32_t TOUCH_TOP_Y = 960;
const int32_t TOUCH_BOTTOM_Y = 140;
const int32_t BUTTON_PADDING = 10;

void setupScreen()
{
  uint16_t ID = tft.readID();
  tft.begin(ID);

  tft.fillScreen(BLACK);
  tft.setCursor(40, 50);
  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  tft.print("Embedded4");

  drawPreviousButton(BUTTON_GAP, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, WHITE);
  drawPlayButton(BUTTON_WIDTH + 2 * BUTTON_GAP, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, WHITE);
  // drawPauseButton(BUTTON_WIDTH + 2 * BUTTON_GAP, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, WHITE);
  drawNextButton(BUTTON_WIDTH * 2 + 3 * BUTTON_GAP, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, WHITE);
}

void setupMp3Player()
{
  mp3Serial.begin(9600);

#ifdef DEBUG
  Serial.println();
  Serial.println("DFRobot DFPlayer Mini Demo");
  Serial.println("Initializing DFPlayer ... (May take 3~5 seconds)");
#endif

  mp3Player.setTimeOut(5000);
  if (!mp3Player.begin(mp3Serial)) // Use softwareSerial to communicate with mp3.
  {
#ifdef DEBUG
    Serial.println("Unable to begin:");
    Serial.println("1.Please recheck the connection!");
    Serial.println("2.Please insert the SD card!");
#endif

    while (true)
    {
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
#ifdef DEBUG
  Serial.println("DFPlayer Mini online.");
#endif

  mp3Player.volume(30); // Set volume value. From 0 to 30
}

void drawPlayButton(int x, int y, int width, int height, uint16_t color)
{
  tft.fillRect(x, y, width, height, BLACK);
  tft.drawRect(x, y, width, height, color);
  tft.fillTriangle(x + width * 19 / 100, y + BUTTON_PADDING, x + width * 19 / 100, y + height - BUTTON_PADDING, x + width * 81 / 100, y + height / 2, color);
}

void drawPauseButton(int x, int y, int width, int height, uint16_t color)
{
  tft.fillRect(x, y, width, height, BLACK);
  tft.drawRect(x, y, width, height, color);
  tft.fillRect(x + width * 12 / 100, y + BUTTON_PADDING, width * 28 / 100, height - BUTTON_PADDING * 2, color);
  tft.fillRect(x + width * 60 / 100, y + BUTTON_PADDING, width * 28 / 100, height - BUTTON_PADDING * 2, color);
}

void drawPreviousButton(int x, int y, int width, int height, uint16_t color)
{
  tft.drawRect(x, y, width, height, color);

  tft.fillRect(x + width * 12 / 100, y + BUTTON_PADDING, width * 28 / 100, height - BUTTON_PADDING * 2, color);
  tft.fillTriangle(x + width * 88 / 100, y + BUTTON_PADDING, x + width * 88 / 100, y + height - BUTTON_PADDING, x + width * 26 / 100, y + height / 2, color);
}

void drawNextButton(int x, int y, int width, int height, uint16_t color)
{
  tft.drawRect(x, y, width, height, color);

  tft.fillRect(x + width * 60 / 100, y + BUTTON_PADDING, width * 28 / 100, height - BUTTON_PADDING * 2, color);
  tft.fillTriangle(x + width * 12 / 100, y + BUTTON_PADDING, x + width * 12 / 100, y + height - BUTTON_PADDING, x + width * 74 / 100, y + height / 2, color);
}

uint8_t getTappedButton()
{
  TSPoint p = ts.getPoint();
  
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
  {
    // #ifdef DEBUG
    // Serial.print("X = ");
    // Serial.print(p.x);
    // Serial.print("\tY = ");
    // Serial.print(p.y);
    // Serial.print("\tPressure = ");
    // Serial.println(p.z);
    // #endif

    int16_t screenX = (p.x - TOUCH_LEFT_X) * SCREEN_WIDTH / (TOUCH_RIGHT_X - TOUCH_LEFT_X);
    int16_t screenY = SCREEN_HEIGHT - (p.y - TOUCH_BOTTOM_Y) * SCREEN_HEIGHT / (TOUCH_TOP_Y - TOUCH_BOTTOM_Y);

    // #ifdef DEBUG
    // Serial.print("SX = ");
    // Serial.print(screenX);
    // Serial.print("\tSY = ");
    // Serial.println(screenY);
    // #endif

    if (screenY >= BUTTON_Y && screenY <= BUTTON_Y + BUTTON_HEIGHT)
    {
      if (screenX >= BUTTON_GAP && screenX <= BUTTON_GAP + BUTTON_WIDTH)
      {
        return PREVIOUS_BUTTON;
      }
      if (screenX >= BUTTON_WIDTH + 2 * BUTTON_GAP && screenX <= 2 * BUTTON_GAP + 2 * BUTTON_WIDTH)
      {
        return PAUSE_PLAY_BUTTON;
      }
      if (screenX >= 2 * BUTTON_WIDTH + 3 * BUTTON_GAP && screenX <= 3 * BUTTON_GAP + 3 * BUTTON_WIDTH)
      {
        return NEXT_BUTTON;
      }
    }
  }

  return 0;
}

void updateScreen()
{
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (isPlaying != pauseDisplayed)
  {
    if (isPlaying)
    {
      drawPauseButton(BUTTON_WIDTH + 2 * BUTTON_GAP, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, WHITE);
    }
    else
    {
      drawPlayButton(BUTTON_WIDTH + 2 * BUTTON_GAP, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, WHITE);
    }
    pauseDisplayed = isPlaying;
  }

  if (currentSongNumber != displayedSong)
  {
    tft.fillRect(0, BUTTON_Y + BUTTON_HEIGHT + 2, SCREEN_WIDTH, 100, BLACK);
    tft.setCursor(20, BUTTON_Y + BUTTON_HEIGHT + 20);
    tft.setTextSize(3);
    tft.setTextColor(WHITE);
    tft.print(artistNames[currentSongNumber]);

    tft.setCursor(20, BUTTON_Y + BUTTON_HEIGHT + 20 + 40);
    tft.print(songNames[currentSongNumber]);

    displayedSong = currentSongNumber;
  }
}

void nextSong()
{
  currentSongNumber = (currentSongNumber + 1) % NUMBER_OF_SONGS;
  mp3Player.next();
  isPaused = false;
  isPlaying = true;
}

void previousSong()
{
  currentSongNumber = (currentSongNumber - 1 + NUMBER_OF_SONGS) % NUMBER_OF_SONGS;
  mp3Player.previous();
  isPaused = false;
  isPlaying = true;
}

void playPause()
{
  if (isPlaying)
  {
    mp3Player.pause();
    isPaused = true;
    isPlaying = false;
  }
  else if (isPaused)
  {
    mp3Player.start();
    isPaused = false;
    isPlaying = true;
  }
  else
  {
    currentSongNumber = (currentSongNumber + 1) % NUMBER_OF_SONGS;
    mp3Player.next();
    isPlaying = true;
    isPaused = false;
  }
}

#ifdef DEBUG
void printDetail(uint8_t type, int value)
{
  switch (type)
  {
  case TimeOut:
    Serial.println("Time Out!");
    break;
  case WrongStack:
    Serial.println("Stack Wrong!");
    break;
  case DFPlayerCardInserted:
    Serial.println("Card Inserted!");
    break;
  case DFPlayerCardRemoved:
    Serial.println("Card Removed!");
    break;
  case DFPlayerCardOnline:
    Serial.println("Card Online!");
    break;
  case DFPlayerUSBInserted:
    Serial.println("USB Inserted!");
    break;
  case DFPlayerUSBRemoved:
    Serial.println("USB Removed!");
    break;
  case DFPlayerPlayFinished:
    Serial.print("Number:");
    Serial.print(value);
    Serial.println(" Play Finished!");
    break;
  case DFPlayerError:
    Serial.print("DFPlayerError:");
    switch (value)
    {
    case Busy:
      Serial.println("Card not found");
      break;
    case Sleeping:
      Serial.println("Sleeping");
      break;
    case SerialWrongStack:
      Serial.println("Get Wrong Stack");
      break;
    case CheckSumNotMatch:
      Serial.println("Check Sum Not Match");
      break;
    case FileIndexOut:
      Serial.println("File Index Out of Bound");
      break;
    case FileMismatch:
      Serial.println("Cannot Find File");
      break;
    case Advertise:
      Serial.println("In Advertise");
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void printMp3PlayerUpdates()
{
  if (mp3Player.available())
  {
    printDetail(mp3Player.readType(), mp3Player.read()); // Print the detail message from DFPlayer to handle different errors and states.
  }
}
#endif