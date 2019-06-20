//  TH-OWL, Bachelor Technische Informatik, 4. Semester, Projektarbeit Prof. Korte
//  Authors: A. Niggemann, J. Illigens, P. Tadday
//  Date: 02.05.2019

#ifndef MotorControl_hpp
#include <arduino.h>
#define MotorControl_hpp
#define MOTORSHIELD_IN1  5
#define MOTORSHIELD_IN2 6
#define MOTORSHIELD_IN3 7
#define MOTORSHIELD_IN4 8
#define SPEEDPIN_A    9
#define SPEEDPIN_B    10
#define ACTIVATION_THRESHHOLD 30
#define DEACTIVATION_THRESHHOLD 15



struct MotorConfig{
  uint8_t speedPin;
  uint8_t forwardPin;
  uint8_t backwardPin;
};

class Motor;

/*
Is used as an interface in client code to control the two motors add once
 */
class MotorControl
{
  public:
  MotorControl(MotorConfig* left, MotorConfig* right);
  void drive(int speed, int rotation);
  
  private:
  Motor* left;
  Motor* right;
};

#endif
