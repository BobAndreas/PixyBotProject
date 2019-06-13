#include "BoundedPID.hpp"

BoundedPID::BoundedPID(PID_Config* config, int initial, int lower, int upper){
    pid = new PID(config);
    lowerBound = lower;
    upperBound = upper;
    setCurrent(initial);
}
void BoundedPID::clearBuf(){
    pid->clearBuf();
}

int BoundedPID::setCurrent(int value){
    if(value > upperBound)
        current = upperBound;
    else if(value < lowerBound)
        current = lowerBound;
    else
        current = value;
    return current;    
}

int BoundedPID::next(int value){
    int potentialNext = pid->next(value) + current;
    return setCurrent(potentialNext);
    
}

