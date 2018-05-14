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

class looper  {

    int counterRecord = 0;
    int loopLengthInSamples = 88200;
    bool loopRecord = false;
    double myLoop[89000];
    int counterPlay = 0;
    bool loopPlay = false;
    double volume = 0.0;
    int index;
    
    public:
    double myLoopOutput[512];
    double recordLoop(int loopLength, int audioIndex, double signal[89000], bool recording);
    double playLoop(bool playing, double vol);
    
};

#endif /* looper_hpp */
