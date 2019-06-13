//  TH-OWL, Bachelor Technische Informatik, 4. Semester, Projektarbeit Prof. Korte
//  Authors: A. Niggemann, J. Illigens, P. Tadday
//  Date: 02.05.2019



#ifdef unix
#include "src/BoundedPID.hpp"
#include "src/MotorControl.hpp"

#else 
#include "src\BoundedPID.hpp"
#include "src\MotorControl.hpp"
#include "src\Helpers.cpp"
#endif

enum State {Waiting, Searching, Following};

MotorControl motorcontrol;

Pixy2 pixyCore;
Pixy2CCC<Link2SPI> pixy = pixyCore.ccc; 

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
const int PAN_LIMIT = 1000;


int following_x, following_y;



void initializePIDControllers(){
  PanController = new BoundedPID(new PID_Config{
    p:30,
    i: 10,
    d: 0,
    target: PAN_LIMIT / 2,
    lowerBound: -200,
    upperBound: 200},
    500, //initial value
    50, //lowerBound
    950); // upperBound

  TiltController = new BoundedPID(new PID_Config{
    p: 30,
    i: 10,
    d: 0,
    target: pixyCore.frameHeight / 2,
    divider: 55,
    lowerBound: -200,
    upperBound: 200}, 
    500, //initial value
    50, //lowerBound
    950); // upperBound

  SpeedController = new PID(new PID_Config{
    p: -20,
    i: -0,
    d: -0,
    target: TARGETSIZE,
    divider: 10,
    lowerBound: -127,
    upperBound: 127});

  RotationController  = new PID(new PID_Config{
    p: 20,
    i: 10,
    d: 0,
    target: pixyCore.frameWidth / 2,
    divider: 20,
    lowerBound: -50,
    upperBound: 50});
}


void setup()
{
  /*Configure the motor A to control the wheel at the right side.*/
  /*Configure the motor B to control the wheel at the left side.*/

  
  Serial.begin(9600);
  for(int i = 5; i < 11; i++) pinMode(i, OUTPUT);
  ////print("Starting...\n");
  pixyCore.init();

  motorcontrol = 
    MotorControl(
      new Motor(
        SPEEDPIN_B,
        MOTORSHIELD_IN4,
        MOTORSHIELD_IN3),
      new Motor(
        SPEEDPIN_A,
        MOTORSHIELD_IN1,
        MOTORSHIELD_IN2));

  initializePIDControllers();

  
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
        //update the current value in the controllers because they have been moved in the searching phase
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
  int pan, tilt;

  //find the biggest block in our simple case
  Block current = findSingleBlockRepresentation(blocks, pixy.blocks);

  //detemines the perceivedsize, which is measured against the targetsize as a 
  //measure of distance for the speed controller, as a simple maximum of both 
  //side length.
  int perceivedsize = max(current.m_height, current.m_width);

  ////println("Combined: ");
  //printBlock(&current);  

  //feed Infos to the relevant controllers
  tilt = TiltController->next(current.m_y);
  pan = PanController->next(current.m_x);

  speedBuff = SpeedController->next(perceivedsize);
  rotationBuff = RotationController->next(pan);

  //calculate the speed for the different tires
  speedLeft = speedBuff + rotationBuff;
  speedRight = speedBuff - rotationBuff;

  //print resulting commands
  /*
  print("left: ");
  println(speedLeft);
  print("right: ");
  println(speedRight);
  print("tilt: ");
  println(tilt);
  println();
   */
  //give the commands to the motorcontrol and transmit it via SPI to the pixy
  motorcontrol.drive(speedLeft, speedRight);
  pixyCore.setServos(pan, tilt);

}

//idleing bevor taking further actions when no blocks are detected
void waiting() {
  idleCount++;
  motorcontrol.drive(0,0);
  delay(100);
}

//search routine to find a block after waiting has been proven futile
void searching() {
  pixyCore.setServos(following_x, following_y);
  motorcontrol.drive(0,0);
  //the search routine moves the pixy camera to different points to search the 
  //whole field of view of the pixy camera
  if ( following_x < 900)
    following_x += 267;
  else {
    if (following_y <= 900)
      following_y += 400;
    else
      following_y = 100;

    following_x = 100;
    //extra delay because the pixy has to travel to the opposite side
    delay(200);
  }
  //wait so that the pixy can pick up a new target
  delay(500);

}






