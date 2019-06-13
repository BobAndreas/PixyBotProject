//  TH-OWL, Bachelor Technische Informatik, 4. Semester, Projektarbeit Prof. Korte
//  Authors: A. Niggemann, J. Illigens, P. Tadday
//  Date: 02.05.2019

#include "PID.hpp"

//Konstruktor
PID::PID(PID_Config* config){
  this->config = config;
  //Die Variable historyScale wird verwendet um die Gewichtung beim Integrativen Teil des Reglers auszugeichen.
  historyScale = 1;
  //Fakultät(historyScale)
  for(int i = 2; i <= BUFFER_SIZE; i++)
    historyScale *= i;
  clearBuf();
}

//Errechnet den nächsten Regelwert "currentValue" und gint diesen zurück. Der Wert "measured" ist der neuste gemessene Wert.
int PID::next(int measured){
  int difference = measured - config->target;
  int sum = 0;
  //Errechnet die Summe der Werte aus dem Ringbuffer "valueBuf[]" mit einer Gewichtung (neuester Wert->höchste Gewichtung)
  for(int i = 0; i < BUFFER_SIZE; i++){
    sum += valueBuf[(i + index)%BUFFER_SIZE] * (BUFFER_SIZE - i);
  }
  
  currentValue = (config->p * difference + config->i * sum / historyScale + config->d * (difference - valueBuf[index])) / config->divider;

  //Vermeidung von extremen Werten: currentValue wird nicht größer als upperBound und nicht kleiner als lowerBound 
  if (currentValue < config->lowerBound)
    currentValue = config->lowerBound;
  else if (currentValue > config->upperBound)
    currentValue = config->upperBound;
 
  index = (index + 1) % BUFFER_SIZE;
  
  valueBuf[index] = difference;
  
  return currentValue;
}

//Gibt aktuellen Wert zurück
int PID::current(){
  return currentValue;
}

//Setzt den Zielwert des Reglers neu
void PID::setTarget(int target){
  config->target = target;
}

//Löscht den Werte Buffer und setzt den Regler zurück
void PID::clearBuf(){
  index = 0;
  currentValue = 0;
  for(int i = 0; i < BUFFER_SIZE; i++){
    valueBuf[i] = 0;
  } 
}
