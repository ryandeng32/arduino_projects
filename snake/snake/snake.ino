#include <LCD5110_Graph.h>
LCD5110 myGLCD(2, 3, 4, 5, 6);
int snakeX = 20;
int snakeY = 20;

int buttonLeft = 12;
int buttonRight = 9;
int buttonUp = 11;
int buttonDown = 10;
int buttonLeftState = 0;
int buttonRightState = 0;
int buttonUpState = 0;
int buttonDownState = 0;

int dir = 1;// 1: up, 2: down, 3: left, 4: right
int snakeLen = 1;
int snakeWidth = 3;
int snakeXArray[50];
int snakeYArray[50];

int foodX, foodY; 


void setup() {
  // put your setup code here, to run once:
  myGLCD.InitLCD();
  pinMode(buttonLeft, INPUT);
  pinMode(buttonRight, INPUT);
  pinMode(buttonUp, INPUT);
  pinMode(buttonDown, INPUT);
  Serial.begin(9600);
  snakeXArray[0] = snakeX;
  snakeYArray[0] = snakeY;
  generateFood(); 
}

void generateFood(){
  foodX = random(1,83); 
  foodY = random(1,47); 
  
}
void displaySnake() {
  for (int i = 0; i < snakeLen; i++)
  {
    myGLCD.drawRect(snakeXArray[i], snakeYArray[i], snakeXArray[i] + snakeWidth - 1, snakeYArray[i]+snakeWidth - 1);
  }
}

void eatFood() {
  if (abs(snakeX - foodX) <= 1 && abs(snakeY - foodY) <= 1){
    snakeLen += 1; 
    generateFood(); 
  }
}
int moveUp(int obj, int unit) {
  obj -= unit;
  return obj;
}
int moveDown(int obj,int unit) {
  obj += unit;
  return obj;
}
int moveLeft(int obj, int unit) {
  obj -= unit;
  return obj;
}
int moveRight(int obj, int unit) {
  obj += unit;
  return obj; 
}
void loop() {
  // put your main code here, to run repeatedly:
  myGLCD.clrScr();
  displaySnake();
  buttonLeftState = digitalRead(buttonLeft);
  buttonRightState = digitalRead(buttonRight);
  buttonUpState = digitalRead(buttonUp);
  buttonDownState = digitalRead(buttonDown);


  if (buttonLeftState == HIGH) {
    dir = 3;
  }
  else if (buttonRightState == HIGH) {
    dir = 4;
  }
  else if (buttonUpState == HIGH) {
    dir = 1;
  }
  else if (buttonDownState == HIGH) {
    dir = 2;
  }

  switch (dir) {
    case 1:
      snakeY = moveUp(snakeY, snakeWidth - 1);
      break;
    case 2:
      snakeY = moveDown(snakeY, snakeWidth - 1);
      break;
    case 3:
      snakeX = moveLeft(snakeX, snakeWidth - 1);
      break;
    case 4:
      snakeX = moveRight(snakeX, snakeWidth - 1);
      break;
  }
  for (int i = 49; i > 0; i--)
  {
    snakeXArray[i] = snakeXArray[i-1]; 
    snakeYArray[i] = snakeYArray[i-1]; 
  }
   snakeXArray[0] = snakeX;
  snakeYArray[0] = snakeY;
  eatFood(); 
  myGLCD.drawCircle(foodX, foodY, 1); 

  delay(90);
  myGLCD.update();
}
