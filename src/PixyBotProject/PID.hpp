//  TH-OWL, Bachelor Technische Informatik, 4. Semester, Projektarbeit Prof. Korte
//  Authors: A. Niggemann, J. Illigens, P. Tadday
//  Date: 02.05.2019
#ifndef PID_hpp
#define PID_hpp

#define BUFFER_SIZE 5

struct PID_Config {
  int p, i, d, target, divider, lowerBound, upperBound;   
        
};

class PID{
  
  public:

    PID(PID_Config* config);
    int next(int measured);
    int current();
    void setTarget(int target);
    void clearBuf();

  private:

    PID_Config* config;
    int currentValue;
    
    //int valueBuf[BUFFER_SIZE];
    int maxIntegral;
    int integral;
    int lastDifference;
    //int index;if(value > upperBound)
        current = upperBound;
    else if(value < lowerBound)
        current = lowerBound;
    else
        current = value;
    //int historyScale;

};

#endif