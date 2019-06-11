//  TH-OWL, Bachelor Technische Informatik, 4. Semester, Projektarbeit Prof. Korte
//  Authors: A. Niggemann, J. Illigens, P. Tadday
//  Date: 02.05.2019



#ifdef unix
#include "src/BoundedPID.hpp"
#include "src/MotorControl.hpp"
#include "src/Helpers.cpp"
#else 
#include "src\BoundedPID.hpp"
#include "src\MotorControl.hpp"
#include "src\Helpers.cpp"
#endif

enum State {Waiting, Searching, Following};

MotorControl motorcontrol = 
  MotorControl(
    new Motor(
      SPEEDPIN_B,
      MOTORSHIELD_IN4,
      MOTORSHIELD_IN3),
    new Motor(
      SPEEDPIN_A,
      MOTORSHIELD_IN1,
      MOTORSHIELD_IN2));

Pixy2 pixyCore;
Pixy2CCC<Link2SPI> pixy = pixyCore.ccc; 

int signature = 0;    //Nr of signature
int x = 0;            //positon x axis
int y = 0;            //position y axis
int width = 0;        //object's width
int height = 0;       //object's height

State state;

BoundedPID* TiltController;
BoundedPID* PanController;
PID* SpeedController;
PID* RotationController;

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

  
  Serial.begin(9600);
  for(int i = 5; i < 11; i++) pinMode(i, OUTPUT);
  ////print("Starting...\n");
  pixyCore.init();
  PanController = new BoundedPID(PID_Config{
    p:30,
    i: 10,
    d: 0,
    target: PAN_LIMIT / 2,
    lowerBound: -200,
    upperBound: 200},
    500, 
    50, 
    950);
  TiltController = new BoundedPID(PID_Config{
    p: 30,
    i: 10,
    d: 0,
    target: pixyCore.frameHeight / 2,
    divider: 55,
    lowerBound: -200,
    upperBound: 200}, 
    500, 
    50, 
    9500);
  SpeedController = new PID(PID_Config{
    p: -20,
    i: -0,
    d: -0,
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
  uint16_t blocks = pixy.getBlocks();  //receive data from pixy

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
        PanController->setCurrent(following_x);
        TiltController->setCurrent(following_y);
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
      println("Searching");
      searching();
      break;
    case Following:
      println("Following");
      following(blocks);
      break;
    case Waiting:
      println("Waiting");
      waiting();
        break;
    }
}

void following(uint16_t blocks) {

  int16_t speedBuff, rotationBuff;
  int16_t speedLeft, speedRight;
  uint16_t maxSize, index;

  //Größtes Rechteck finden
  Block current = findSingleBlockRepresentation(blocks, pixy.blocks);

  ////println("Combined: ");
  printBlock(&current);
  x = current.m_x;                    //get x position
  y = current.m_y;                    //get y position
  width = current.m_width;            //get width
  height = current.m_height;          //get height



  

  tilt = TiltController->next(y);
  pan = PanController->next(x);

  int perceivedsize = max(height, width);
  speedBuff = SpeedController->next(perceivedsize);
  rotationBuff = RotationController->next(pan);

  speedLeft = speedBuff + rotationBuff;
  speedRight = speedBuff - rotationBuff;
  
  print("left: ");
  println(speedLeft);
  print("right: ");
  println(speedRight);
  print("tilt: ");
  println(tilt);
  println();
  motorcontrol.drive(speedLeft, speedRight);
  pixyCore.setServos(pan, tilt);

}


void waiting() {
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






