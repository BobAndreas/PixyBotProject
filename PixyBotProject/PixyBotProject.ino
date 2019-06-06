//  TH-OWL, Bachelor Technische Informatik, 4. Semester, Projektarbeit Prof. Korte
//  Authors: A. Niggemann, J. Illigens, P. Tadday
//  Date: 02.05.2019

#include "PID.h"
#include "MotorControl.h"
#include "Pixy2\Pixy2.h"
#include "Pixy2\Pixy2CCC.h"

enum State {Waiting, Searching, Following};

MotorControl motorcontrol = MotorControl(new Motor(
                              SPEEDPIN_B,
                              MOTORSHIELD_IN4,
                              MOTORSHIELD_IN3),
                            new Motor(
                              SPEEDPIN_A,
                              MOTORSHIELD_IN1,
                              MOTORSHIELD_IN2));

Pixy2 pixyCore;
Pixy2CCC<Link2SPI> pixy = pixyCore.ccc; //Pixy2CCC<Link2SPI>(&pixyCore);

int signature = 0;    //Nr of signature
int x = 0;            //positon x axis
int y = 0;            //position y axis
int width = 0;        //object's width
int height = 0;       //object's height

State state;

PID* TiltController;
PID* SpeedController;
PID* RotationController;

Block last;
bool lastValid;
int tilt = 0;
int idleCount = 0;

const int TARGETSIZE = 90;
const int SPEED_FACTOR = 2;
const int CLEAR_COUNT = 30;
const int SPEED_LIMIT = 200;

int following_x, following_y;




void setup()
{
  /*Configure the motor A to control the wheel at the right side.*/
  /*Configure the motor B to control the wheel at the left side.*/

  lastValid = false;
  Serial.begin(9600);
  for(int i = 5; i < 11; i++)pinMode(i, OUTPUT);
  ////Serial.print("Starting...\n");
  pixyCore.init();

  TiltController = new PID(PID_Config{
    p: 30,
    i: 10,
    d: 0,
    target: pixyCore.frameHeight / 2,
    divider: 55,
    lowerBound: -450,
    upperBound: 450});
  SpeedController = new PID(PID_Config{
    p: 20,
    i: 00,
    d: 0,
target: TARGETSIZE,
    divider: 10,
lowerBound: -127,
    upperBound: 127});
  RotationController  = new PID(PID_Config{
    p: 20,
    i: 10,
    d: 0,
    target: pixyCore.frameWidth / 2,
    divider: 20,
    lowerBound: -50,
    upperBound: 50});
  state = Waiting;
}

void loop()
{
  uint16_t blocks;


  blocks = pixy.getBlocks();  //receive data from pixy

  switch (state) {
    case Waiting:
      if (blocks > 0) {
        state = Following;
        idleCount = 0;
        break;
      } else {
        if (idleCount > CLEAR_COUNT) {
          idleCount = 0;
          state = Searching;
          TiltController->clearBuf();     //clears buffer
          SpeedController->clearBuf();    //clears buffer
          RotationController->clearBuf(); //clears buffer
          following_x = 100;
          following_y = 100;
          break;
        }
      }
      break;
    case Searching:
      if (blocks > 0) {
        state = Following;
      }
      break;
    case Following:
      if (blocks <= 0) {
        state = Waiting;
      }
      break;

  }
  
  switch (state) {
    case Searching:
      Serial.println("Searching");
      searching();
      break;
    case Following:
      Serial.println("Following");
      following(blocks);
      break;
    case Waiting:
      Serial.println("Waiting");
      waiting();
        break;
    }
}

void following(uint16_t blocks) {

  int16_t speedBuff, rotationBuff;
  int16_t speedLeft, speedRight;
  signature = pixy.blocks[0].m_signature;    //get object's signature
  uint16_t maxSize, index;

  //Größtes Rechteck finden
  Block current = findBlockRepresentation(blocks, pixy.blocks);

  if (lastValid)
    current = fusion(&current, &last);

  ////Serial.println("Combined: ");
  printBlock(&current);
  x = current.m_x;                    //get x position
  y = current.m_y;                    //get y position
  width = current.m_width;            //get width
  height = current.m_height;          //get height


  last = current;
  lastValid = true;

  tilt += TiltController->next(y);
  if(tilt > 450) tilt = 450;
  if(tilt < -450) tilt = -450;


  speedBuff = CalcSpeed(width, height);
  rotationBuff = RotationController->next(x);

  speedLeft = speedBuff + rotationBuff;
  speedRight = speedBuff - rotationBuff;
  
  Serial.print("left: ");
  Serial.println(speedLeft);
  Serial.print("right: ");
  Serial.println(speedRight);
  Serial.print("tilt: ");
  Serial.println(tilt);
  Serial.println();
  motorcontrol.drive(speedLeft, speedRight);
  pixyCore.setServos(500, 500 + tilt);

}


void waiting() {
  lastValid = false;
  idleCount++;
  motorcontrol.drive(0,0);
  delay(100);
}

void searching() {
  pixyCore.setServos(following_x, following_y);
  motorcontrol.drive(0,0);
  if ( following_x < 900)
    following_x += 267;
  else {
    if (following_y < 900)
      following_y += 400;
    else
      following_y = 100;

    following_x = 100;
    delay(200);
  }
  delay(500);

}
void printBlock(Block* block) {
  //Serial print single values
  //Print signature
  //Serial.print("Signature = ");
  //Serial.println(block->m_signature);

  //print x position
  Serial.print("x = ");
  Serial.println(block->m_x);

  //print y position
  //Serial.print("y = ");
  //Serial.println(block->m_y);

  //print tha wirdth of the object
  Serial.print("width = ");
  Serial.println(block->m_width);

  //print the height of the object
  Serial.print("height = ");
  Serial.println(block->m_height);

  Serial.print("perceived size = ");
  Serial.println(max(block->m_height, block->m_width));
}

Block findBlockRepresentation(int16_t blockcount, Block* blocks) {
  int16_t maxSize, index;
  maxSize = -1;
  for (int i = 0; i < blockcount; i++) {
    int16_t curMax = max(blocks[i].m_width, blocks[i].m_height);
    if (curMax > maxSize) {
      index = i;
      maxSize = curMax;
    }
  }
  return blocks[index];
}


int16_t combine(int16_t prop1, int16_t prop2) {
  return (prop1 * 2 + prop2) / 3;
}

Block fusion(Block* current, Block* last) {
  Block combined;
  combined.m_x = combine(current->m_x, last->m_x);
  combined.m_y = combine(current->m_y, last->m_y);
  combined.m_width = combine(current->m_width, last->m_width);
  combined.m_height = combine(current->m_height, last->m_height);
  return combined;
}

int16_t CalcSpeed(int width, int height) {
  int perceivedsize = max(height, width);
  return -(SpeedController->next(perceivedsize));

}