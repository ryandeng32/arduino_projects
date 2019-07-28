// Pong game
// by Ryan Deng
// 27/7/2019
//
// This program is a remake of the arcade pong game
// It has both single player mode and multiplayer mode
//
#include <LCD5110_Graph.h>
#include "pitches.h"


LCD5110 myGLCD(2, 3, 4, 5, 6);

// extern some of the font from library
extern uint8_t MediumNumbers[];
extern uint8_t SmallFont[];
extern uint8_t TinyFont[];

int button1Pin = 12;  // pad1 up button
int button2Pin = 11;  // pad1 down button
int button3Pin = 8; // pad2 up button
int button4Pin = 7; // pad2 down button
int button1State = 0;
int button2State = 0;
int button3State = 0;
int button4State = 0;

int twistPin = A5; // A potentiometer used to control pad in single player
int readValue = 0; // used to record the value from potentiometer


int buzzer = 10; // buzzer pin

int height = 10; // pad height
int width = 2; // pad width
int pad1[2] = {8, 10}; // the inital position of pad1, 7 unit away from border
int pad2[2] = {75 - (width + 1), 10}; // the initial position of pad2, 7 unit away from border

int score1 = 0; // player1's score
int score2 = 0; // player2's score

int ball[2] = {10, 30}; // ball's initial position

// 1: up, 2: down, 3: left, 4: right, 5: up-left, 6: up-right, 7: down-left, 8: down-right
int leftDir[] = {3, 5, 7};
int rightDir[] = {4, 6, 8};
int ballDir = 8; // ball's initial direction

int mode = 0; // 0: game start menu, 1: multiplayer, 2: single player, 3: game end screen

void setup()
{
  myGLCD.InitLCD();
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
  pinMode(button4Pin, INPUT);
  pinMode(twistPin, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  myGLCD.clrScr();
  if (mode == 0) {
    startMenu();
  }
  if (mode == 1) {
    delay(20);
    drawHUD();
    drawPlayer(pad1);
    drawPlayer(pad2);

    // move players based on button input
    button1State = digitalRead(button1Pin);
    button2State = digitalRead(button2Pin);
    button3State = digitalRead(button3Pin);
    button4State = digitalRead(button4Pin);
    if (button1State == HIGH) {
      moveUp(pad1, 1);
    } else if (button2State == HIGH) {
      moveDown(pad1, 1);
    } else if (button3State == HIGH) {
      moveUp(pad2, 1);
    } else if (button4State == HIGH) {
      moveDown(pad2, 1);
    }

    restrict(pad1);
    restrict(pad2);

    updateBall();
    collideBall();

    if (score1 >= 3 || score2 >= 3) {
      mode = 3;  // game over
    }
  }
  if (mode == 2) {
    delay(10);
    drawHUD();
    readValue = analogRead(twistPin); // control pad1 via potentiometer
    pad1[1] = readValue / 28;
    drawPlayer(pad1);

    if (ballDir == 4 || ballDir == 6 || ballDir == 8){ // how the robot opponent behaves
      if (pad2[1] < ball[1]-2 && random(10) != 0)
        moveDown(pad2, 1); 
      else 
        moveUp(pad2, 1); 
    }
    drawPlayer(pad2);

    restrict(pad1);
    restrict(pad2);

    updateBall();
    collideBall();

    if (score1 >= 3 || score2 >= 3) {
      mode = 3;  // game over
    }
  }
  if (mode == 3)
    endScreen();

  myGLCD.update();
}

void startMenu() // display start menu, finished
{
  myGLCD.setFont(SmallFont);
  myGLCD.print("PONG", CENTER, 2);
  myGLCD.drawRect(0, 0, 83, 47);
  myGLCD.drawLine(0, 10, 84, 10);
  myGLCD.setFont(TinyFont);
  myGLCD.print("Black. Multiplayer", CENTER, 20);
  myGLCD.print("Blue. Single Player", CENTER, 30);

  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);
  if (button1State == HIGH) {
    mode = 1;
    myGLCD.clrScr();
  } else if (button2State == HIGH) {
    mode = 2;
    myGLCD.clrScr();
  }
}

void endScreen() // display end screen, finished
{
  myGLCD.setFont(SmallFont);
  myGLCD.print("Game Over", CENTER, 2);
  if (score1 > score2)
    myGLCD.print("P1 Wins!", CENTER, 15);
  else
    myGLCD.print("P2 Wins!", CENTER, 15);
  myGLCD.drawRect(0, 0, 83, 47);
  myGLCD.drawLine(0, 10, 84, 10);
  myGLCD.setFont(TinyFont);
  myGLCD.print("Red. Replay", CENTER, 25);

  button3State = digitalRead(button3Pin);   
  if (button3State == HIGH) {  // reset the whole game and return to start menu
    mode = 0;
    score1 = 0; 
    score2 = 0; 
    myGLCD.clrScr(); 
  }
}

void moveUp(int pad[2], int unit)
{
  pad[1] -= unit;
}
void moveDown(int pad[2], int unit)
{
  pad[1] += unit;
}
void moveLeft(int ball[2], int unit)
{
  ball[0] -= unit;
}
void moveRight(int ball[2], int unit)
{
  ball[0] += unit;
}


void restrict(int pad[2]) // make sure the pads won't move out of the border, finished
{
  if (pad[1] < 1)
    pad[1] = 1;
  if (pad[1] > 47 - height)
    pad[1] = 47 - height;
}

void drawHUD() // draw the grid of the game, finished
{
  myGLCD.drawRect(0, 0, 83, 47); // the border

  for (int i = 0; i < 48; i += 4) {
    // drawing the net
    myGLCD.drawLine(42, i, 42, i + 2);
  }

  // displaying score
  myGLCD.setFont(MediumNumbers);
  myGLCD.printNumI(score1, 24, 5);
  myGLCD.printNumI(score2, 49, 5);
}

void drawPlayer(int pad[2]) // draw the player, finished
{
  // draw the outline of player
  myGLCD.drawRect(pad[0], pad[1], pad[0] + width - 1, pad[1] + height - 1);

  // fill the shape of player by drawing lines inside of the outline
  for (int i = 1; i <= width - 2; i++) {
    myGLCD.drawLine(pad[0] + i, pad[1] + 1, pad[0] + i, pad[1] + height - 1);
  }
}

void collideBall() // checks the collision between players and the ball, finished
{
  if (ball[1] >= pad1[1] - 1 && ball[1] <= pad1[1] + height + 1 && ball[0] <= 42)
  {
    if (ball[0] == pad1[0]) {
      if (ball[1] - pad1[1] <= height / 2)
        ballDir = 8;
      else
        ballDir = 6;
    }
  }
  if (ball[1] >= pad2[1] - 1 && ball[1] <= pad2[1] + height + 1 && ball[0] >= 42)
  {
    if (ball[0] == pad2[0]) {
      if (ball[1] - pad2[1] <= height / 2)
        ballDir = 7;
      else
        ballDir = 5;
    }
  }
}

void win(int player) // execute when either player win, finished
{
  if (player == 1)
  {
    // add score
    score1++;

    // play sound
    int noteDuration = 1000 / 4;
    tone(10, NOTE_C4, noteDuration);
    delay(noteDuration * 1.30);
    noTone(10);

    // change ball's direction and location
    ball[0] = 80;
    ball[1] = random(84);
    ballDir = leftDir[random(3)];
  }
  else if (player == 2)
  {
    score2++;

    int noteDuration = 1000 / 4;
    tone(10, NOTE_G3, noteDuration);
    delay(noteDuration * 1.30);
    noTone(10);

    ball[0] = 8;
    ball[1] = random(84);
    ballDir = rightDir[random(3)];
  }
}

void updateBall() // update the ball's position, direction, and check if either player wins a point
{
  myGLCD.drawCircle(ball[0], ball[1], 1);
  switch (ballDir)
  {
    case 1:
      moveUp(ball, 1);
      break;
    case 2:
      moveDown(ball, 1);
      break;
    case 3:
      moveLeft(ball, 1);
      break;
    case 4:
      moveRight(ball, 1);
      break;
    case 5:
      moveUp(ball, 1);
      moveLeft(ball, 1);
      break;
    case 6:
      moveUp(ball, 1);
      moveRight(ball, 1);
      break;
    case 7:
      moveDown(ball, 1);
      moveLeft(ball, 1);
      break;
    case 8:
      moveDown(ball, 1);
      moveRight(ball, 1);
      break;
    default:
      break;
  }

  if (ball[1] < 1) // bounce on the top border
  {
    ball[1] = 1;
    switch (ballDir) // change direction
    {
      case 1:
        ballDir = 2;
        break;
      case 5:
        ballDir = 7;
        break;
      case 6:
        ballDir = 8;
        break;
      default:
        break;
    }
  }
  if (ball[1] > 46) // bounce on the bottom border
  {
    ball[1] = 46;
    switch (ballDir)  // change direction
    {
      case 2:
        ballDir = 1;
        break;
      case 7:
        ballDir = 5;
        break;
      case 8:
        ballDir = 6;
        break;
      default:
        break;
    }
  }
  // one player will win when the ball reaches the left or the right border
  if (ball[0] < 1)
    win(2);
  if (ball[0] > 82)
    win(1);
}
