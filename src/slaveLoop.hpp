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
    double myLooped[882300];
    int counterPlay = 0;
    bool loopPlay = false;
    int index;
    int loopTracker, loopTracker2;
    float vol;
    
public:
    double myLoopOutput[520];
    double recordLoop(int loopLength, int audioIndex, double signal[44200], bool recording, int slave);
    double playLoop(bool playing, float amp);
    
    // this loopTracker3 is public so that it can be used to control playback of other loops
    int loopTracker3;
    
};

#endif /* slaveLoop_hpp */

