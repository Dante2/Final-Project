//
//  looper.cpp
//  oscSenderExample
//
//  Created by Jools on 02/05/2018.
//

#include "slaveLoop.hpp"

using std::cout;
using std::endl;
using std::cin;
using std::boolalpha;

//-------- RECORD LOOP --------//

double slaveLoop::recordLoop(int loopLength, int audioIndex, double signal[], bool recording, int slave){
    index = audioIndex;
    
    // loopLengthInSamples is set in app.cpp. It's 44100 multiplied by some int to give usd a desired recording in seconds
    loopLengthInSamples = loopLength;
    
    // boolean activates recording
    loopRecord = recording;
    if (loopRecord == true) {
        // set the counter going
        counterRecord ++;
        
        loopTracker = slave;
        
        // loopTracker2 is being set to the length dictated by slave which is being fed by the tracker in loop1 and will be used in playback
        loopTracker2 = loopTracker;
        if (counterRecord > loopLengthInSamples) {
            counterRecord = 0;
        }
        myLoop[counterRecord] = signal[index];
        myLoop[counterRecord + 1] = signal[index + 1];
    } else if (loopRecord == false){
        counterRecord = 0;
        loopTracker = 0;
    }
}

// NEED TO FLUSH OUT THE LOOP SO THAT EACH TIME WE RECORD A NEW LOOP NO REMNANTS OF THE OLD LOOP REMAIN

//-------- PLAY LOOP --------//

double slaveLoop::playLoop(bool playing, float amp){
    vol = amp;
    loopPlay = playing;
    if (loopPlay == true){
        counterPlay ++;
        
        // functionality above similar to that of record function however here playback is limited by time set by loopTracker2
        if (counterPlay > loopTracker2) {
            counterPlay = 0;
        }
        myLoopOutput[index] = myLoop[counterPlay] * vol;
    } else if (loopPlay == false){
        counterPlay = 0;
    }
}

