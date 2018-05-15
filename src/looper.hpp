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
    int loopLengthInSamples = 44200;
    bool loopRecord = false;
    double myLoop[882500];
    int counterPlay = 0;
    bool loopPlay = false;
    double volume = 0.0;
    int index;
    int loopRecorded = 0;
    
    public:
    double myLoopOutput[520];
    double recordLoop(int loopLength, int audioIndex, double signal[89000], bool recording);
    double playLoop(bool playing, double vol);
    
};

#endif /* looper_hpp */
