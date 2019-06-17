//  TH-OWL, Bachelor Technische Informatik, 4. Semester, Projektarbeit Prof. Korte
//  Authors: A. Niggemann, J. Illigens, P. Tadday
//  Date: 02.05.2019

#ifndef MotorControl_hpp
#define MotorControl_hpp
#define MOTORSHIELD_IN1  5
#define MOTORSHIELD_IN2 6
#define MOTORSHIELD_IN3 7
#define MOTORSHIELD_IN4 8
#define SPEEDPIN_A    9
#define SPEEDPIN_B    10
#define ACTIVATION_THRESHHOLD 30
#define DEACTIVATION_THRESHHOLD 15



enum Movement {Forward, Standing, Backwards};

/*
Abstraction over a single Motor
 */
class Motor{
  public:

  Motor(uint8_t analog, uint8_t digital1, uint8_t digital2);
  void drive(int speed);
  
  private:
  uint8_t analog;
  uint8_t digital1;
  uint8_t digital2;

  Movement direction;
  
};

/*
Is used as an interface in client code to control the two motors add once
 */
class MotorControl
{
  public:
  MotorControl(Motor* left, Motor* right);
  void drive(int speedLeft, int speedRight);
  
  private:
  Motor* left;
  Motor* right;
};

#endif