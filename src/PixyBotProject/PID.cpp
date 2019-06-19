//  TH-OWL, Bachelor Technische Informatik, 4. Semester, Projektarbeit Prof. Korte
//  Authors: A. Niggemann, J. Illigens, P. Tadday
//  Date: 02.05.2019

#include "PID.hpp"

PID::PID(PID_Config* config){
  this->config = config;
  maxIntegral = 2000;
  clearBuf();
}
//restrict values to be between set bounds;
int limit(int value, int lowerBound, int upperBound){
  if(value > upperBound)
    return upperBound;
  else if(value < lowerBound)
    return lowerBound;
  return value;
}

//updates the currentValue depending on the newest measured value
int PID::next(int measured){
  int difference = measured - config->target;
  integral = limit(integral + difference, -maxIntegral, maxIntegral);
  
  currentValue = limit((difference * config->p + integral * config->i + (difference-lastDifference) * config->d) / config->divider, config->lowerBound, config->upperBound);
  
  lastDifference = difference;
  return currentValue;
}

int PID::current(){
  return currentValue;
}

void PID::setTarget(int target){
  config->target = target;
}

//resets the PID
void PID::clearBuf(){
  currentValue = 0;
  integral = 0;
  lastDifference = 0;
  
}
