//
//  looper.cpp
//  oscSenderExample
//
//  Created by Jools on 02/05/2018.
//

#include "looper.hpp"

using std::cout;
using std::endl;
using std::cin;
using std::boolalpha;

//-------- RECORD LOOP --------//

void looper::triggerRecord(bool switchOnRecord){
    loopRecord = switchOnRecord;
}

double looper::recordLoop(int loopLength, int audioIndex, double signal[]){
    index = audioIndex;
    // cout << "record index = " << index << endl;
    loopLengthInSamples = loopLength;
    // loopRecord = recording;
    if (loopRecord == true) {
        counterRecord ++;
        // cout << "loop counter = " << counterRecord << endl;
        if (counterRecord > loopLengthInSamples) {
            counterRecord = 0;
        }
        myLoop[counterRecord] = signal[index];
        myLoop[counterRecord + 1] = signal[index + 1];
        cout << "loop recorded L = " << myLoop[counterRecord] << endl;
        cout << "loop recorded R = " << myLoop[counterRecord + 1] << endl;
    } else if (loopRecord == false){
        counterRecord = 0;
    }
}

void looper::triggerPlayBack(bool switchOnPlay){
    loopPlay = switchOnPlay;
}

//-------- PLAY LOOP --------//

double looper::playLoop(bool playing){
    loopPlay = playing;
    if (loopPlay == true){
        counterPlay ++;
        // cout << "play counter = " << counterPlay << endl;
        if (counterPlay > loopLengthInSamples) {
            counterPlay = 0;
        }
        myLoopOutput[index] = myLoop[counterPlay];
        cout << "loop playing = " << myLoopOutput[counterPlay] << endl;
    } else if (loopPlay == false){
    counterPlay = 0;
    }
}

