#ifndef Mp3Player_h
#define Mp3Player_h

void setupScreen();
void setupMp3Player();
void updateScreen();

uint8_t getTappedButton();

void nextSong();
void previousSong();
void playPause();
void printMp3PlayerUpdates();

#define PREVIOUS_BUTTON 1
#define NEXT_BUTTON 2
#define PAUSE_PLAY_BUTTON 3

#endif