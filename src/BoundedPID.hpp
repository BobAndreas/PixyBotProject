#include "PID.hpp"


class BoundedPID {
    PID pid;
    int current, lowerBound, upperBound;

    public:
    BoundedPID(PID_Config config, int initial, int lower, int upper);
    int next(int measured);
    void clearBuf();
    int setCurrent(int current);
};