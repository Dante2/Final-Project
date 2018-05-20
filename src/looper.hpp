//
//  looper.hpp
//  oscSenderExample
//
//  Created by Jools on 02/05/2018.
//

#ifndef looper_hpp
#define looper_hpp

#include <stdio.h>
#include <iostream>
#include "ofxMaxim.h"

// using namespace std;
/* Looper */

class looper  {

    int counterRecord = 0;
    int loopLengthInSamples = 882100;
    bool loopRecord = false;
    double myLoop[882200];
    int counterPlay = 0;
    bool loopPlay = false;
    int index;
    float vol;
    maxiDelayline loop;
    
public:
    double myLoopOutput[520];
    double recordLoop(int loopLength, int audioIndex, double signal[44200], bool recording);
    double playLoop(bool playing, float amp);
    int loopTracker;
    bool playIt;
//    double LoopThrough[520];
//    void triggerRecord();
    
};

#endif /* looper_hpp */
