#define MOTORSHIELD_IN1  5
#define MOTORSHIELD_IN2 6
#define MOTORSHIELD_IN3 7
#define MOTORSHIELD_IN4 8
#define SPEEDPIN_A    9
#define SPEEDPIN_B    10
#define ACTIVATION_THRESHHOLD 30
#define DEACTIVATION_THRESHHOLD 15

enum Movement {Forward, Standing, Backwards};


class Motor{
  public:
  Motor(uint8_t, uint8_t,uint8_t);
  void drive(int speed);
  
  uint8_t analog;
  uint8_t digital1;
  uint8_t digital2;

  
  Movement direction;
  
};
Motor::Motor(uint8_t analog1, uint8_t digital11, uint8_t digital21){
  this->analog = analog11;
  this->digital1 = digital111;
  this->digital2 = digital211;
}


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
    if (forward){
      analogWrite(analog, speed); //* m.correction);
    }
    else{
      analogWrite(analog, -speed); //* m.correction);
    }
   }
}


class MotorControl
{
  public:
  MotorControl(Motor* left, Motor* right);
  void drive(int speedLeft, int speedRight);
  
  private:
  Motor* left;
  Motor* right;
};

MotorControl::MotorControl(Motor* left, Motor* right){
  Serial.println("Surprise motherfucker");
  Serial.println(left->analog);
  Serial.println(left->digital1);
  Serial.println(left->digital2);
  Serial.println(right->analog);
  Serial.println(right->digital1);
  Serial.println(right->digital2);
  
  this->left = left;
  this->right = right;
}



void MotorControl::drive(int speedLeft, int speedRight){
  //Hystereseschaltung für die Aktivierung der Motoren
  left->drive(speedLeft);
  right->drive(speedRight);
}
