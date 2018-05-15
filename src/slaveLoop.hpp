//
//  looper.hpp
//  oscSenderExample
//
//  Created by Jools on 02/05/2018.
//

#ifndef slaveLoop_hpp
#define slaveLoop_hpp

#include <stdio.h>
#include <iostream>

// using namespace std;
/* slave Looper */

class slaveLoop  {
    
    int counterRecord = 0;
    int loopLengthInSamples = 882100;
    bool loopRecord = false;
    double myLoop[882200];
    int counterPlay = 0;
    bool loopPlay = false;
    int index;
    int loopTracker2;
    
public:
    double myLoopOutput[520];
    double recordLoop(int loopLength, int audioIndex, double signal[44200], bool recording, int slave);
    double playLoop(bool playing);
    int loopTracker;
    //    void triggerRecord();
    
};

#endif /* slaveLoop_hpp */

