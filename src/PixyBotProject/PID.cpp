//  TH-OWL, Bachelor Technische Informatik, 4. Semester, Projektarbeit Prof. Korte
//  Authors: A. Niggemann, J. Illigens, P. Tadday
//  Date: 02.05.2019

#include "PID.hpp"

PID::PID(PID_Config* config){
  
  this->config = config;
  historyScale = 1;
  for(int i = 2; i <= BUFFER_SIZE; i++)
    historyScale *= i;
  clearBuf();
}

int PID::next(int measured){
  int difference = measured - config->target;
  int sum = 0;
  for(int i = 0; i < BUFFER_SIZE; i++){
    sum += valueBuf[(i + index)%BUFFER_SIZE] * (BUFFER_SIZE - i);
  }
  
  currentValue = (config->p * difference + config->i * sum / historyScale + config->d * (difference - valueBuf[index])) / config->divider;

  if (currentValue < config->lowerBound) currentValue = config->lowerBound;
  else if (currentValue > config->upperBound) currentValue = config->upperBound;
 
  index = (index + 1) % BUFFER_SIZE;
  
  valueBuf[index] = difference;
  
  return currentValue;
}

int PID::current(){
  return currentValue;
}

void PID::setTarget(int target){
  config->target = target;
}

void PID::clearBuf(){

  index = 0;
  currentValue = 0;
  for(int i = 0; i < BUFFER_SIZE; i++){
    valueBuf[i] = 0;
  } 
}