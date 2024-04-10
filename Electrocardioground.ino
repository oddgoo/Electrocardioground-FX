#include <Arduboy2.h>
#include <ArduboyTones.h>
// #include <EEPROM.h>
// navigation state


#define TITLE 0
#define GAME 1
#define DEATH 2
#define COMPLETE 3
byte gamestate = TITLE;

#define TOP 1
#define BOTTOM -1

#define GENTLE 0
#define MAYHEM 1

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

const int column_width = 6;
const int max_column_life = 127;
const int max_column_height = 32 - column_width;
const float depletion_speed = 4;
const float recovery_speed = 2;

const int MAX_BEATS = 200; // Maximum number of beats
const int GENTLE_TARGET_SCORE = 200; // Maximum number of beats

int death_mode = 0; //0=crash, 1=depletion

byte helperVarTest = 99; //0x63 0b1100011
byte gameMode = GENTLE;

int titleOption = 0;
char colorA = 0;
char colorB = 1;

byte crashTolerance = 2;

byte beatGap = 0;
byte gentleBeatGap = 6;
byte initialBeatGap = 5;

char gentleCourse[]       = {1,0,1,1,0,0,-1,1,0, 1,1,0,0,-1,-1, 1,1,0, 1,0,1, 1,0,-1, 1,0,1,-1,1,-1};
char gentleCourseDir[]    = {1,0,1,1,1,0, 1,1,0,-1,1,0,0,-1,-1, 1,1,0,-1,0,1,-1,0,-1,-1,0,1, 1,1, 1};
int gentleCourseIndex = 0;

//const int EEPROM_HIGH_SCORE_ADDRESS = 0;

const uint8_t PROGMEM titleA[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x08, 0x08, 0x08, 0x08, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x08, 0x08, 0x08, 0x08, 0x00, 0xf8, 0x08, 0x08, 0x08, 0x08, 0x00, 0x18, 0x18, 0xf8, 0x18, 0x18, 0x00, 0xf8, 0x08, 0x08, 0x08, 0xf8, 0x00, 0xf8, 0x08, 0x08, 0x08, 0xf8, 0x00, 0xf8, 0x08, 0x08, 0x08, 0x08, 0x00, 0xf0, 0x08, 0x08, 0x08, 0xf0, 0x00, 0xf8, 0x08, 0x08, 0x08, 0xf8, 0x00, 0xf8, 0x08, 0x08, 0x08, 0xf0, 0x00, 0x08, 0x08, 0xf8, 0x08, 0x08, 0x00, 0xf8, 0x08, 0x08, 0x08, 0xf8, 0x00, 0xf8, 0x08, 0x08, 0x08, 0x08, 0x00, 0xf8, 0x08, 0x08, 0x08, 0xf8, 0x00, 0xf8, 0x08, 0x08, 0x08, 0xf8, 0x00, 0xf8, 0x00, 0x00, 0x00, 0xf8, 0x00, 0xf8, 0x80, 0x00, 0x00, 0xf8, 0x00, 0xf8, 0x08, 0x08, 0x08, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0xff, 0x18, 0x18, 0x18, 0x18, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x18, 0x18, 0x18, 0x18, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x1e, 0x66, 0x86, 0x07, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x06, 0x06, 0x06, 0xff, 0x00, 0xff, 0x1e, 0x66, 0x86, 0x07, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x18, 0x18, 0xf8, 0x00, 0xff, 0x1e, 0x66, 0x86, 0x07, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x03, 0x3c, 0xc0, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x0f, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x0f, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x0f, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x01, 0x0e, 0x00, 0x0f, 0x0e, 0x0e, 0x0e, 0x0f, 0x00, 0x0f, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x0f, 0xc0, 0x40, 0x40, 0x4f, 0x00, 0x4f, 0x80, 0x00, 0x81, 0x4e, 0x00, 0x0f, 0x0e, 0x0e, 0x0e, 0x07, 0x00, 0x0e, 0x0e, 0x0f, 0x0e, 0x0e, 0x00, 0x0f, 0x0e, 0x0e, 0x0e, 0x0f, 0x00, 0x0f, 0x0e, 0x0e, 0x0e, 0x0f, 0x00, 0x0f, 0x00, 0x00, 0x01, 0x0e, 0x00, 0x0f, 0x0e, 0x0e, 0x0e, 0x0f, 0x00, 0x0f, 0x0e, 0x0e, 0x0e, 0x0f, 0x00, 0x0f, 0x00, 0x00, 0x07, 0x0f, 0x00, 0x0f, 0x0e, 0x0e, 0x0e, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x03, 0x02, 0x02, 0x02, 0x00, 0x08, 0x04, 0x03, 0x07, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
};

const uint8_t PROGMEM titleB[] = {
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xf7, 0xf7, 0xf7, 0xf7, 0xff, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xf7, 0xf7, 0xf7, 0xf7, 0xff, 0x07, 0xf7, 0xf7, 0xf7, 0xf7, 0xff, 0xe7, 0xe7, 0x07, 0xe7, 0xe7, 0xff, 0x07, 0xf7, 0xf7, 0xf7, 0x07, 0xff, 0x07, 0xf7, 0xf7, 0xf7, 0x07, 0xff, 0x07, 0xf7, 0xf7, 0xf7, 0xf7, 0xff, 0x0f, 0xf7, 0xf7, 0xf7, 0x0f, 0xff, 0x07, 0xf7, 0xf7, 0xf7, 0x07, 0xff, 0x07, 0xf7, 0xf7, 0xf7, 0x0f, 0xff, 0xf7, 0xf7, 0x07, 0xf7, 0xf7, 0xff, 0x07, 0xf7, 0xf7, 0xf7, 0x07, 0xff, 0x07, 0xf7, 0xf7, 0xf7, 0xf7, 0xff, 0x07, 0xf7, 0xf7, 0xf7, 0x07, 0xff, 0x07, 0xf7, 0xf7, 0xf7, 0x07, 0xff, 0x07, 0xff, 0xff, 0xff, 0x07, 0xff, 0x07, 0x7f, 0xff, 0xff, 0x07, 0xff, 0x07, 0xf7, 0xf7, 0xf7, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xff, 0xff, 0xff, 0x00, 0xe7, 0xe7, 0xe7, 0xe7, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xe7, 0xe7, 0xe7, 0xe7, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xe1, 0x99, 0x79, 0xf8, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xf9, 0xf9, 0xf9, 0x00, 0xff, 0x00, 0xe1, 0x99, 0x79, 0xf8, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xe7, 0xe7, 0x07, 0xff, 0x00, 0xe1, 0x99, 0x79, 0xf8, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xfc, 0xc3, 0x3f, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xe7, 0xe7, 0xe7, 0xe7, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xf1, 0xf1, 0xf1, 0xf1, 0xff, 0xf0, 0xf1, 0xf1, 0xf1, 0xf1, 0xff, 0xf0, 0xf1, 0xf1, 0xf1, 0xf1, 0xff, 0xf0, 0xf1, 0xf1, 0xf1, 0xf1, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xfe, 0xf1, 0xff, 0xf0, 0xf1, 0xf1, 0xf1, 0xf0, 0xff, 0xf0, 0xf1, 0xf1, 0xf1, 0xf1, 0xff, 0xf0, 0x3f, 0xbf, 0xbf, 0xb0, 0xff, 0xb0, 0x7f, 0xff, 0x7e, 0xb1, 0xff, 0xf0, 0xf1, 0xf1, 0xf1, 0xf8, 0xff, 0xf1, 0xf1, 0xf0, 0xf1, 0xf1, 0xff, 0xf0, 0xf1, 0xf1, 0xf1, 0xf0, 0xff, 0xf0, 0xf1, 0xf1, 0xf1, 0xf0, 0xff, 0xf0, 0xff, 0xff, 0xfe, 0xf1, 0xff, 0xf0, 0xf1, 0xf1, 0xf1, 0xf0, 0xff, 0xf0, 0xf1, 0xf1, 0xf1, 0xf0, 0xff, 0xf0, 0xff, 0xff, 0xf8, 0xf0, 0xff, 0xf0, 0xf1, 0xf1, 0xf1, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xfc, 0xfd, 0xfd, 0xfd, 0xff, 0xf7, 0xfb, 0xfc, 0xf8, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const uint8_t PROGMEM infoA[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x40, 0xc0, 0x00, 0xc0, 0x40, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x40, 0x00, 0xc0, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x40, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x40, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x40, 0x80, 0x00, 0x00, 0x00, 0xc0, 0x00, 0xc0, 0x40, 0x00, 0x40, 0xc0, 0x40, 0x00, 0xc0, 0x40, 0xc0, 0x00, 0xc0, 0x40, 0xc0, 0x40, 0xc0, 0x00, 0xc0, 0x40, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x40, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x40, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x01, 0x06, 0x00, 0x07, 0x01, 0x07, 0x00, 0x07, 0x00, 0x05, 0x07, 0x00, 0x07, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x01, 0x07, 0x00, 0x03, 0x04, 0x03, 0x00, 0x07, 0x04, 0x07, 0x00, 0x07, 0x00, 0x07, 0x04, 0x03, 0x00, 0x04, 0x03, 0x00, 0x00, 0x05, 0x07, 0x00, 0x00, 0x07, 0x00, 0x00, 0x07, 0x04, 0x07, 0x00, 0x07, 0x00, 0x03, 0x00, 0x07, 0x00, 0x07, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x07, 0x00, 0x07, 0x04, 0x07, 0x00, 0x07, 0x01, 0x06, 0x00, 0x03, 0x04, 0x03, 0x00, 0x07, 0x00, 0x03, 0x04, 0x03, 0x00, 0x07, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x27, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0xe0, 0x00, 0x00, 0xa0, 0x40, 0x00, 0x00, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xeb, 0xe0, 0x00, 0xa0, 0x40, 0x00, 0xfc, 0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0d, 0x0b, 0x0d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xdf, 0xbf, 0xf8, 0x08, 0x08, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xfd, 0xfa, 0xff, 0xf8, 0x28, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x80, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x80, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xf8, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x9f, 0x5f, 0x9f, 0x3f, 0xff, 0x3f, 0x1f, 0x9f, 0x1f, 0x3f, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0xff, 0x3f, 0xdf, 0x5f, 0x9f, 0x3f, 0xff, 0x3f, 0x1f, 0x1f, 0x1f, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xf7, 0xf1, 0xf7, 0xf8, 0xff, 0xf8, 0xf1, 0xf0, 0xf1, 0xf8, 0xff, 0xfd, 0xfe, 0xff, 0xff, 0xff, 0xf8, 0xf7, 0xf5, 0xf2, 0xf8, 0xff, 0xf8, 0xf1, 0xf2, 0xf1, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xd7, 0xc7, 0xff, 0xa7, 0xdf, 0xe7, 0xff, 0xff, 0xff, 0xc7, 0xd7, 0xc7, 0xff, 0xc7, 0xd7, 0xc1, 0xff, 0xc7, 0xd7, 0xc1, 0xff, 0x47, 0x57, 0x07, 0xff, 0xc7, 0xd7, 0xc7, 0xff, 0xc7, 0xd7, 0xc7, 0xff, 0xff, 
};

const uint8_t PROGMEM infoB[] = {
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xbf, 0x3f, 0xff, 0x3f, 0xbf, 0x3f, 0xff, 0x3f, 0xff, 0x3f, 0xbf, 0xff, 0x3f, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xbf, 0x3f, 0xff, 0x3f, 0xff, 0x3f, 0xff, 0x3f, 0xbf, 0x3f, 0xff, 0x3f, 0xff, 0x3f, 0xbf, 0x7f, 0xff, 0xff, 0xff, 0x3f, 0xff, 0x3f, 0xbf, 0xff, 0xbf, 0x3f, 0xbf, 0xff, 0x3f, 0xbf, 0x3f, 0xff, 0x3f, 0xbf, 0x3f, 0xbf, 0x3f, 0xff, 0x3f, 0xbf, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xbf, 0xff, 0x3f, 0xff, 0x3f, 0xff, 0x3f, 0xbf, 0x3f, 0xff, 0x3f, 0xff, 0x3f, 0xff, 0x3f, 0xff, 0x3f, 0xff, 0x3f, 0xff, 0x3f, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xfe, 0xf9, 0xff, 0xf8, 0xfe, 0xf8, 0xff, 0xf8, 0xff, 0xfa, 0xf8, 0xff, 0xf8, 0xfa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xfe, 0xf8, 0xff, 0xfc, 0xfb, 0xfc, 0xff, 0xf8, 0xfb, 0xf8, 0xff, 0xf8, 0xff, 0xf8, 0xfb, 0xfc, 0xff, 0xfb, 0xfc, 0xff, 0xff, 0xfa, 0xf8, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xf8, 0xfb, 0xf8, 0xff, 0xf8, 0xff, 0xfc, 0xff, 0xf8, 0xff, 0xf8, 0xfd, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0xf8, 0xff, 0xf8, 0xfb, 0xf8, 0xff, 0xf8, 0xfe, 0xf9, 0xff, 0xfc, 0xfb, 0xfc, 0xff, 0xf8, 0xff, 0xfc, 0xfb, 0xfc, 0xff, 0xf8, 0xfa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x43, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb8, 0xd8, 0xb8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x1f, 0x1f, 0xff, 0xff, 0x5f, 0xbf, 0xff, 0xff, 0xf8, 0xf8, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x14, 0x1f, 0xff, 0x5f, 0xbf, 0xff, 0x03, 0x03, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2, 0xf4, 0xf2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x20, 0x40, 0x07, 0xf7, 0xf7, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x02, 0x05, 0x00, 0x07, 0xd7, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x7f, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x7f, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x60, 0xa0, 0x60, 0xc0, 0x00, 0xc0, 0xe0, 0x60, 0xe0, 0xc0, 0x00, 0x00, 0x00, 0x80, 0x40, 0x00, 0xc0, 0x20, 0xa0, 0x60, 0xc0, 0x00, 0xc0, 0xe0, 0xe0, 0xe0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x08, 0x0e, 0x08, 0x07, 0x00, 0x07, 0x0e, 0x0f, 0x0e, 0x07, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x07, 0x08, 0x0a, 0x0d, 0x07, 0x00, 0x07, 0x0e, 0x0d, 0x0e, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x28, 0x38, 0x00, 0x58, 0x20, 0x18, 0x00, 0x00, 0x00, 0x38, 0x28, 0x38, 0x00, 0x38, 0x28, 0x3e, 0x00, 0x38, 0x28, 0x3e, 0x00, 0xb8, 0xa8, 0xf8, 0x00, 0x38, 0x28, 0x38, 0x00, 0x38, 0x28, 0x38, 0x00, 0x00, 
};

const uint16_t winSong[] PROGMEM = {
  320,150, 400,150, 320,150, 400,150, 630,350,
  TONES_END };

// columns
class Column
{
public:
  Column() : life(max_column_life), state(false), x(0), h(TOP) {}
  Column(byte x, int h) : life(max_column_life), state(false), x(x), h(h) {}

  int h; // which half
  int life;
  bool state; // true for extended, false for retracted
  byte x;
  int height()
  {
    return (float)life / max_column_life * max_column_height;
  }

  void updateState(bool buttonPressed, int depletionSpeed, int maxLife, bool (*checkSquash)(int, int)) {
    if (buttonPressed) {

      if(this->state == false){
         sound.tone(600, 50);
      }

      this->state = true;
      if (this->life > 0) {
        this->life -= depletionSpeed;
      }
    } else {
      if (this->state == true) {
        this->state = false;
     
        if (checkSquash(this->x, this->h)) {
            sound.tone(200, 80, 250, 80);
        } else {
            sound.tone(200, 50);
        }
      }
      if (this->life < maxLife) {
        this->life += recovery_speed;
      }
    }
  }
  
};

// Beat class
class Beat
{
public:
  Beat() : x(0), h(TOP), d(0) {}
  Beat(byte x, int h, int d) : x(x), h(h), d(d) {}

  byte x;
  int h; // Half (TOP or BOTTOM)
  int d; // Direction (1 for right, -1 for left)
  bool collidesWith(Column &column) {
    if (this->x >= column.x-column_width+crashTolerance && this->x < column.x + column_width-crashTolerance) {
      if (this->h == column.h && !column.state) {
        return true;
      }
    }
    return false;
  }
};

Beat beats[MAX_BEATS];
int numBeats = 0;

Column columns[2];

// game vars
byte helperVarTestB = 127; //0x7F 0b1111111
int score = 0;
int speed = 0;
bool beatStomped = false;
word t = 0;

int gentleHighScore = 0;
int mayhemHighScore = 0;

void initialiseGlobals()
{
  gentleCourseIndex = 0;
  beatGap = initialBeatGap;
  score = 0;
  t = 0;
  beatStomped = false;
  columns[0] = Column((WIDTH / 2) - column_width*2, TOP);
  columns[1] = Column((WIDTH / 2) + column_width*2, BOTTOM);
}

void setup()
{
  // EEPROM.get(EEPROM_HIGH_SCORE_ADDRESS, gentleHighScore); Not doing high-score saving right now as there are possible EEPROM collisions to manage
  // EEPROM.get(EEPROM_HIGH_SCORE_ADDRESS + sizeof(int), mayhemHighScore);

  initialiseGlobals();
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.initRandomSeed();
}

void loop()
{
  if (!(arduboy.nextFrame()))
  {
    return;
  }
  arduboy.pollButtons();
  arduboy.clear();

  switch (gamestate)
  {

  case TITLE:
    title_screen();
    break;

  case GAME:
    game();
    break;

  case DEATH:
    lose_screen();
    break;

  case COMPLETE:
    win_screen();
    break;

  }
  arduboy.display();

}

void title_screen()
{
  int maxOptions = 3;

  //Choose Mode
  if(arduboy.justPressed(RIGHT_BUTTON)){
    titleOption++;
    if(titleOption> maxOptions-1)
      titleOption = 0;
  }
    //Choose Mode
  if(arduboy.justPressed(LEFT_BUTTON)){
    titleOption--;
    if(titleOption<0)
      titleOption = 2;
  }

  if(titleOption!=2){ //show title if not on instructions
    if(colorA == BLACK)
      arduboy.drawBitmap(0, 0, titleA, 128, 64, WHITE);  
    else
      arduboy.drawBitmap(0, 0, titleB, 128, 64, WHITE);  
  }

  if(titleOption == 0){
    gameMode = GENTLE;
    arduboy.setCursor(23,44);
    arduboy.setTextColor(colorA);
    arduboy.print("<   Gentle   >");

  } else if(titleOption == 1){
    gameMode = MAYHEM;
    arduboy.setCursor(19,44);
    arduboy.setTextColor(colorA);
    arduboy.print("<   ENDLESS   >");
  } else if(titleOption == 2){
    if(colorA == BLACK)
      arduboy.drawBitmap(0, 0, infoA, 128, 64, WHITE);  
    else
      arduboy.drawBitmap(0, 0, infoB, 128, 64, WHITE);  
  }

  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(DOWN_BUTTON) ||
      arduboy.justPressed(B_BUTTON) || arduboy.justPressed(UP_BUTTON))
      if (titleOption!=2)
        {
          initialiseGlobals();
          gamestate = GAME;
        }
        else
          titleOption = 0;
}

void game()
{
  drawStage();
  processColumns();
  updateBeats();
  drawBeats();
  if(t%6==0) score++;
  arduboy.setCursor(0, 0);
  arduboy.setTextColor(colorB);
  arduboy.print(score);
  t++;
}

void drawStage()
{
  arduboy.fillRect(0, 0, WIDTH, HEIGHT / 2, colorA);          // top half
  arduboy.fillRect(0, HEIGHT / 2, WIDTH, HEIGHT / 2, colorB); // bottom half
}

void processColumns()
{
  updateColumns();
  drawColumns();
}

void updateColumns()
{
  bool buttonPressedA = arduboy.pressed(A_BUTTON) || arduboy.pressed(UP_BUTTON) || arduboy.pressed(LEFT_BUTTON);
  columns[0].updateState(buttonPressedA, depletion_speed, max_column_life, checkSquash);

  bool buttonPressedB = arduboy.pressed(B_BUTTON) || arduboy.pressed(DOWN_BUTTON) || arduboy.pressed(RIGHT_BUTTON);
  columns[1].updateState(buttonPressedB, depletion_speed, max_column_life, checkSquash);

  if(gamestate == GAME)
    if (columns[0].life <= 0 || columns[1].life <= 0 )
    {
      death_mode = 1;
      lose();
    }

}

bool checkSquash(byte cx, int h)
{
  bool squashed = false;
  for (int i = 0; i < numBeats; i++)
  {
    if (beats[i].h == h && beats[i].x > cx - column_width && beats[i].x < cx + column_width)
    {
      beats[i].h = -beats[i].h; // Swap the half (TOP becomes BOTTOM, BOTTOM becomes TOP)
      squashed = true;
      beatStomped = true; //global variable helper for achievement
    }
  }
  return squashed;
}

void lose()
{
  sound.tone(110, 70, 120, 60, 80, 100);
  switchColors();
  // Clear the beats array
  for (int i = 0; i < MAX_BEATS; i++) {
    beats[i] = Beat();
  }
  numBeats = 0;
  gamestate = DEATH;
}

void win(){
  sound.tones(winSong);
    for (int i = 0; i < MAX_BEATS; i++) {
    beats[i] = Beat();
  }
  numBeats = 0;
  gamestate = COMPLETE;
}

void updateScore() {
  // Existing updateScore() function logic...

  // Update the high score if the current score is higher
  // if (gameMode == GENTLE && score > gentleHighScore) {
  //   gentleHighScore = score;
  //   EEPROM.put(EEPROM_HIGH_SCORE_ADDRESS, gentleHighScore); // Save the high score to the EEPROM
  // }
  // else if (gameMode == MAYHEM && score > mayhemHighScore) {
  //   mayhemHighScore = score;
  //   EEPROM.put(EEPROM_HIGH_SCORE_ADDRESS + sizeof(int), mayhemHighScore); // Save the high score to the EEPROM
  // }

  
}

void switchColors(){
  char tempColor = colorA;
  colorA = colorB;
  colorB = tempColor;
}

void drawColumns()
{
  arduboy.fillRect(columns[0].x, HEIGHT / 2 - columns[0].height() - columns[0].state * column_width, column_width, columns[0].height(), colorB);
  arduboy.fillRect(columns[1].x, HEIGHT / 2 + columns[1].state * column_width, column_width, columns[1].height(), colorA);
}

void updateBeats() {
  // Generate new beats
  
  if(gameMode == MAYHEM) {

    beatGap = initialBeatGap - (score / 40);
    if (beatGap < 3) beatGap = 3;
 
    if (t%(column_width*beatGap) == 0) { // Adjust the probability of generating new beats

      int h = random(2) == 0 ? TOP : BOTTOM; // Random half
      int d = random(2) == 0 ? 1 : -1; 
      int x = d == 1 ? 0 : WIDTH - 1; // Starting x-coordinate based on the direction
      if (numBeats < MAX_BEATS) {
        beats[numBeats] = Beat(x, h, d);
        numBeats++;
      }

      // if (t%(column_width*beatGap*4) == 0) //extra mayhem
      //   switchColors();
    }
  } else if (gameMode == GENTLE) {
    if (t % (column_width * gentleBeatGap) == 0 && gentleCourseIndex < sizeof(gentleCourse)){
      if (gentleCourse[gentleCourseIndex] != 0) {
        int h = gentleCourse[gentleCourseIndex] == 1 ? TOP : BOTTOM; // Top or bottom based on gentleCourse
        int d = gentleCourseDir[gentleCourseIndex]; // Direction based on gentleCourseDir
        int x = d == 1 ? 0 : WIDTH - 1; // Starting x-coordinate based on the direction
        if (numBeats < MAX_BEATS) {
          beats[numBeats] = Beat(x, h, d);
          numBeats++;
        }
      }
      gentleCourseIndex++;

    }
        // Check if we've reached the end of the gentleCourse array
    if (score >= GENTLE_TARGET_SCORE) {
      win();
    }
  }

  // Move beats
  for (int i = 0; i < numBeats; i++) {
    beats[i].x += beats[i].d;
    // Remove beats that go off-screen
    if (beats[i].x < 0 || beats[i].x >= WIDTH) {
      beats[i] = beats[numBeats - 1];
      numBeats--;
      i--;
    }
  }

  //collide beats
  for (int i = 0; i < MAX_BEATS; i++) {
    for (int j = 0; j < 2; j++) { // Assuming there are 2 columns
      if (beats[i].collidesWith(columns[j])) {
        // End the game
        death_mode = 0;
        lose();
        return;
      }
    }
  }
}

void drawBeats()
{
  for (int i = 0; i < numBeats; i++)
  {
    byte y = beats[i].h == TOP ? HEIGHT / 2 - column_width : HEIGHT / 2;
    arduboy.fillRect(beats[i].x, y, column_width, column_width, beats[i].h == TOP ? colorB : colorA);
  }
}

void win_screen(){

  drawStage();
  arduboy.setCursor(11, 11);
  arduboy.setTextColor(colorB);
  arduboy.print("Gentle Course Win!");

  arduboy.setCursor(11, 11+33);
  arduboy.setTextColor(colorA);
  arduboy.print("Gentle Course Win!");

  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(DOWN_BUTTON) ||
    arduboy.justPressed(B_BUTTON) || arduboy.justPressed(UP_BUTTON))
  {
    gamestate = TITLE;
  }
}

void lose_screen()
{
  drawStage();
  arduboy.setCursor(48, 11);
  arduboy.setTextColor(colorB);
  if(death_mode == 0)
    arduboy.print("Crash!");
  else
    arduboy.print("Empty!");

  arduboy.setCursor(48, 11+33);
  arduboy.setTextColor(colorA);
  if(death_mode == 0)
    arduboy.print("Crash!");
  else
    arduboy.print("Empty!");

  arduboy.setCursor(0, 0);
  arduboy.setTextColor(colorB);
  arduboy.print(score);

  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(DOWN_BUTTON) ||
      arduboy.justPressed(B_BUTTON) || arduboy.justPressed(UP_BUTTON))
  {
    gamestate = TITLE;
  }

}
