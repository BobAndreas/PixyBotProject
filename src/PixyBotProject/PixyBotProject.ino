//  TH-OWL, Bachelor Technische Informatik, 4. Semester, Projektarbeit Prof. Korte
//  Authors: A. Niggemann, J. Illigens, P. Tadday
//  Date: 02.05.2019



#include <Pixy2.h>
#include <Pixy2CCC.h>
#include "BoundedPID.hpp"
#include "MotorControl.hpp"


enum State {Waiting, Searching, Following};

MotorControl motorcontrol = 
    MotorControl(
      new MotorConfig{
        speedPin: SPEEDPIN_B,
        forwardPin: MOTORSHIELD_IN4,
        backwardPin: MOTORSHIELD_IN3},
      new MotorConfig{
        speedPin: SPEEDPIN_A,
        forwardPin: MOTORSHIELD_IN1,
        backwardPin: MOTORSHIELD_IN2});

Pixy2 pixyCore;
Pixy2CCC<Link2SPI> pixy = Pixy2CCC<Link2SPI>(&pixyCore);


State state;

BoundedPID* TiltController;
BoundedPID* PanController;
PID* SpeedController;
PID* RotationController;

int idleCount = 0;

//for searching mode
int targetindex;
int currentX;
int currentY;

const int TARGETSIZE = 90;
const int SPEED_FACTOR = 2;
const int CLEAR_COUNT = 30;
const int SPEED_LIMIT = 200;
const int PAN_LIMIT = 1000;



//long timer, lasttimer;



void initializePIDControllers(){
  PanController = new BoundedPID(new PID_Config{
    p:-40,
    i: 0,
    d:-40,
    target: pixyCore.frameWidth / 2,
    divider: 100,
    lowerBound: -2000,
    upperBound: 2000},
    PAN_LIMIT / 2, //initial value
    50, //lowerBound
    950); // upperBound

  TiltController = new BoundedPID(new PID_Config{
    p: 40,
    i: 0,
    d: 50,
    target: pixyCore.frameHeight / 2,
    divider: 100,
    lowerBound: -2000,
    upperBound: 2000}, 
    500, //initial value
    50, //lowerBound
    950); // upperBound

  SpeedController = new PID(new PID_Config{
    p: -40,
    i: -0,
    d: -0,
    target: TARGETSIZE,
    divider: 20,
    lowerBound: -127,
    upperBound: 127});

  RotationController  = new PID(new PID_Config{
    p: -15,
    i: -1,
    d: 0,
    target: PAN_LIMIT / 2,
    divider: 50,
    lowerBound: -50,
    upperBound: 50});
}


void setup()
{
  /*Configure the motor A to control the wheel at the right side.*/
  /*Configure the motor B to control the wheel at the left side.*/

  
  //Serial.begin(9600);
  for(int i = 5; i < 11; i++) pinMode(i, OUTPUT);
  ////Serial.print("Starting...\n");
  pixyCore.init();
  pixyCore.changeProg("color_connected_components");

  initializePIDControllers();

  state = Waiting;
  
  pixyCore.setServos(PAN_LIMIT / 2, PAN_LIMIT / 2);
}

void loop()
{
  uint16_t blocks = pixy.getBlocks();  //receive data from pixy

  switch (state) {
    case Waiting:
      if (blocks > 0) {
        state = Following;
        idleCount = 0;
      } 
      else if (idleCount > CLEAR_COUNT) {
        idleCount = 0;
        PanController->clearBuf();      //clears buffer
        TiltController->clearBuf();     //clears buffer
        SpeedController->clearBuf();    //clears buffer
        RotationController->clearBuf(); //clears buffer
        
        currentX = PanController->getCurrent();
        currentY = TiltController->getCurrent();
        targetindex = 0;
        state = Searching;
      }
      break;
    case Searching:
      if (blocks > 0) {
        //update the current value in the controllers because they have been moved in the searching phase

        PanController->setCurrent(currentX);
        TiltController->setCurrent(currentY);

        state = Following;
      }
      break;
    case Following:
      //no block found or an error occured
      if (blocks <= 0) {
        state = Waiting; 
      }
      break;

  }
  
  switch (state) {
    case Searching:
      //Serial.println("Searching");
      searching();
      break;
    case Following:
      //Serial.println("Following");
      following(blocks);
      break;
    case Waiting:
      //Serial.println("Waiting");
      waiting();
        break;
    }
  /* 
    timer = millis();
    
    Serial.println(timer-lasttimer);
    lasttimer = timer;
  */
}

void following(uint16_t blocks) {

  int16_t speedBuff, rotationBuff;
  int16_t speedLeft, speedRight;
  int pan, tilt;

  //find the biggest block in our simple case
  Block current = findSingleBlockRepresentation(blocks, pixy.blocks);

  //detemines the perceivedsize, which is measured against the targetsize as a 
  //measure of distance for the speed controller, as a simple maximum of both 
  //side length.
  int perceivedsize = max(current.m_height, current.m_width);

  //feed Infos to the relevant controllers
  tilt = TiltController->next(current.m_y);
  pan = PanController->next(current.m_x);

  speedBuff = SpeedController->next(perceivedsize);
  rotationBuff = RotationController->next(pan);


  //print resulting commands
  /*
  Serial.print("left: ");
  Serial.println(speedLeft);
  Serial.print("right: ");
  Serial.println(speedRight);
  Serial.print("tilt: ");
  Serial.println(tilt);
  Serial.println();
   */
  //give the commands to the motorcontrol and transmit it via SPI to the pixy
  motorcontrol.drive(speedBuff, rotationBuff);
  pixyCore.setServos(pan, tilt);

}

//idleing before taking further actions when no blocks are detected
void waiting() {
  idleCount++;
  motorcontrol.drive(0,0);
  delay(100);
}

struct Point
{
  int x,y;
};

Point[] targets = { 
  Point{x: 100, y:100},
  Point{x: 900, y:100},
  Point{x: 900, y:366},
  Point{x: 100, y:366},
  Point{x: 100, y:633},
  Point{x: 900, y:633},
  Point{x: 900, y:900},
  Point{x: 100, y:900}};
//search routine to find a block after waiting has been proven futile
void searching() {
  motorcontrol.drive(0,0);


  int diffX = limit(targets[targetindex].x - currentX, -20, 20); 
  int diffY = limit(targets[targetindex].y - currentY, -20, 20);

  currentX = limit(diffX + currentX, 50, 950);
  currentY = limit(diffY + currentY, 50, 950);
  
  pixyCore.setServos(currentX, currentY);

  if(currentX == targets[targetindex].x && currentY == targets[targetindex].y)
    targetindex = (targetindex +1) % (sizeof(target) / sizeof(Point));

}

Block findSingleBlockRepresentation(int16_t blockcount, Block* blocks) {
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

void printBlock(Block* block) {
  
  //Print signature
  Serial.print("Signature = ");
  Serial.println(block->m_signature);

  //print x position
  Serial.print("x = ");
  Serial.println(block->m_x);

  //print y position
  Serial.print("y = ");
  Serial.println(block->m_y);

  //print tha wirdth of the object
  Serial.print("width = ");
  Serial.println(block->m_width);

  //print the height of the object
  Serial.print("height = ");
  Serial.println(block->m_height);

  //print perceived size
  Serial.print("perceived size = ");
  Serial.println(max(block->m_height, block->m_width));

}
