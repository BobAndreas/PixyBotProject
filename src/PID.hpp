#define BUFFER_SIZE 5

struct PID_Config {
  int p, i, d, target, divider, lowerBound, upperBound;   
        
};

class PID{
  
  public:

    PID(PID_Config config);
    int next(int measured);
    int current();
    void setTarget(int target);
    void clearBuf();

  private:

    PID_Config config;
    int currentValue;
    int valueBuf[BUFFER_SIZE];
    int index;
    int historyScale;

};
