#include <Arduboy2.h>
#include <EEPROM.h>
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

const int column_width = 6;
const int max_column_life = 127;
const int max_column_height = 32 - column_width;
const float depletion_speed = 4;
const float recovery_speed = 2;

const int MAX_BEATS = 200; // Maximum number of beats
const int GENTLE_TARGET_SCORE = 200; // Maximum number of beats

String death_message = "Lose!";

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
      this->state = true;
      if (this->life > 0) {
        this->life -= depletionSpeed;
      }
    } else {
      if (this->state == true) {
        this->state = false;
        if (checkSquash(this->x, this->h)) {
          // arduboy.tunes.tone(880, 50);
        } else {
          // arduboy.tunes.tone(220, 50);
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
int score = 0;
int speed = 0;
word t = 0;

int gentleHighScore = 0;
int mayhemHighScore = 0;

void initialiseGlobals()
{
  gentleCourseIndex = 0;
  beatGap = initialBeatGap;
  score = 0;
  t = 0;
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
  drawStage();

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

  if(titleOption == 0){
    gameMode = GENTLE;
    arduboy.setCursor(23,44);
    arduboy.setTextColor(colorA);
    arduboy.print("<   Gentle   >");

  } else if(titleOption == 1){
    gameMode = MAYHEM;
    arduboy.setCursor(23,44);
    arduboy.setTextColor(colorA);
    arduboy.print("<   MAYHEM   >");
  } else if(titleOption == 2){
    arduboy.setCursor(7, 8);
    arduboy.setTextColor(colorB);
    arduboy.print(F("Instructions"));
  }

  if(titleOption!=2){ //show title if not on instructions
    arduboy.setCursor(7, 8);
    arduboy.setTextColor(colorB);
    arduboy.print(F("ELECTROCARDIOGROUND"));
    arduboy.setCursor(57, 18);
    arduboy.print(F("fx"));
  }


  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(DOWN_BUTTON) ||
      arduboy.justPressed(B_BUTTON) || arduboy.justPressed(UP_BUTTON))
      if (titleOption!=2)
        {
          initialiseGlobals();
          gamestate = GAME;
        }

  // high scores
  // arduboy.setTextColor(colorA);
  // arduboy.setCursor(37, 45);
  // arduboy.print(gentleHighScore);

  // arduboy.setTextColor(colorA);
  // arduboy.setCursor(80, 45);
  // arduboy.print(mayhemHighScore);
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
  bool buttonPressedA = arduboy.pressed(A_BUTTON) || arduboy.pressed(DOWN_BUTTON);
  columns[0].updateState(buttonPressedA, depletion_speed, max_column_life, checkSquash);

  bool buttonPressedB = arduboy.pressed(B_BUTTON) || arduboy.pressed(UP_BUTTON);
  columns[1].updateState(buttonPressedB, depletion_speed, max_column_life, checkSquash);

  if(gamestate == GAME)
    if (columns[0].life <= 0 || columns[1].life <= 0 )
    {
      death_message = "Empty!";
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
    }
  }
  return squashed;
}

void lose()
{
  switchColors();
  // Clear the beats array
  for (int i = 0; i < MAX_BEATS; i++) {
    beats[i] = Beat();
  }
  numBeats = 0;
  gamestate = DEATH;
}

void win(){
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

      if (t%(column_width*beatGap*4) == 0) //extra mayhem
        switchColors();
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
        death_message = "Crash!";
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
  arduboy.print(death_message);

  arduboy.setCursor(48, 11+33);
  arduboy.setTextColor(colorA);
  arduboy.print(death_message);

  arduboy.setCursor(0, 0);
  arduboy.setTextColor(colorB);
  arduboy.print(score);

  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(DOWN_BUTTON) ||
      arduboy.justPressed(B_BUTTON) || arduboy.justPressed(UP_BUTTON))
  {
    gamestate = TITLE;
  }

}
