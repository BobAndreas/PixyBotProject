#include "MotorControl.hpp"


Motor::Motor(uint8_t analog, uint8_t digital1, uint8_t digital2){
  this->analog = analog;
  this->digital1 = digital1;
  this->digital2 = digital2;
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
    digitalWrite(digital1, forward);
    digitalWrite(digital2, not forward);
    
    analogWrite(analog, abs(speed)); 
   }
   else {
     analogWrite(analog, 0);
   }
}

MotorControl::MotorControl(Motor* left, Motor* right){
  
  this->left = left;
  this->right = right;
}



void MotorControl::drive(int speedLeft, int speedRight){
  //Hystereseschaltung für die Aktivierung der Motoren
  left->drive(speedLeft);
  right->drive(speedRight);
}
