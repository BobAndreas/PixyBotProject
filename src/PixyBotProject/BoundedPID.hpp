//  TH-OWL, Bachelor Technische Informatik, 4. Semester, Projektarbeit Prof. Korte
//  Authors: A. Niggemann, J. Illigens, P. Tadday
//  Date: 02.05.2019


#include "PID.cpp"

class BoundedPID {
    PID* pid;
    int current, lowerBound, upperBound;

    public:
    BoundedPID(PID_Config* config, int initial, int lower, int upper);
    int next(int measured);
    void clearBuf();
    int setCurrent(int current);
};
