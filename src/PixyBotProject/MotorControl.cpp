#include "MotorControl.hpp"


Motor::Motor(uint8_t speedPin, uint8_t forwardPin, uint8_t backwardPin){
  this->speedPin = speedPin;
  this->forwardPin = forwardPin;
  this->backwardPin = backwardPin;
}

///Holds the main logic for driving a single motor
void Motor::drive(int speed){
  if(speed > ACTIVATION_THRESHHOLD){
    direction = Forward;
  }
  else if(speed < -ACTIVATION_THRESHHOLD){
    direction = Backwards;
  }
  else if(abs(speed) < DEACTIVATION_THRESHHOLD){
    direction = Standing;
  }
  
  if(direction != Standing){  
    bool forward = direction == Forward;
    digitalWrite(forwardPin, forward);
    digitalWrite(backwardPin, not forward);
    
    analogWrite(speedPin, abs(speed)); 
   }
   else {
     analogWrite(speedPin, 0);
   }
}

MotorControl::MotorControl(Motor* left, Motor* right){
  
  this->left = left;
  this->right = right;
}



void MotorControl::drive(int speed, int rotation){
  //Hystereseschaltung für die Aktivierung der Motoren

  //calculate the speed for the different tires
  int speedLeft = speed + rotation;
  int speedRight = speed - rotation;

  left->drive(speedLeft);
  right->drive(speedRight);
}
