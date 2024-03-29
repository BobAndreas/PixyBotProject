#include "BoundedPID.hpp"

//Constructor
BoundedPID::BoundedPID(PID_Config* config, int initial, int lower, int upper){
    pid = new PID(config);
    lowerBound = lower;
    upperBound = upper;
    setCurrent(initial);
}

//clears value buffer
void BoundedPID::clearBuf(){
    pid->clearBuf();
}

//restricts the setting for current to not exeed hardware restrictions (e.g. Pixy2 Pan/Tilt servomotors)
int BoundedPID::setCurrent(int value){
    return current = limit(value, lowerBound, upperBound);    
}

//returns next value with restrictions from setCurrent()
int BoundedPID::next(int measured){
    int potentialNext = pid->next(measured) + current;
    return setCurrent(potentialNext);
}

int BoundedPID::getCurrent(){
    return current;
}
