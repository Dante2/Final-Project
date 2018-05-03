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

double looper::recordLoop(int loopLength, int audioIndex, double signal[], bool recording){
    loopLengthInSamples = loopLength;
    loopRecord = recording;
    if (loopRecord == true) {
        counterRecord ++;
        // cout << "loop counter = " << counterRecord << endl;
        if (counterRecord > loopLengthInSamples) {
            counterRecord = 0;
        }
        myLoop[counterRecord] = signal[audioIndex];
        myLoop[counterRecord + 1] = signal[audioIndex + 1];
        cout << "loop recorded L = " << myLoop[counterRecord] << endl;
//        cout << "loop recorded R = " << myLoop[counterRecord + 1] << endl;
    } else if (loopRecord == false){
        counterRecord = 0;
    }
}

//-------- PLAY LOOP --------//

double looper::playLoop(int audioIndex, bool playing){
    loopPlay = playing;
    if (loopPlay == true){
        counterPlay ++;
        if (counterPlay > loopLengthInSamples) {
            counterPlay = 0;
        }
        myLoopOutputL[counterPlay] = myLoop[audioIndex];
        // myLoopOutputR[counterPlay + 1] = myLoop[audioIndex + 1];
        cout << "loop playing L = " << myLoopOutputL[counterPlay] << endl;
        // return myLoopOutputL[counterPlay] = myLoop[audioIndex];
        //return myLoopOutputR[counterPlay + 1] = myLoop[audioIndex + 1];
        
//        cout << "loop playing R = " << myLoopOutputR[counterPlay + 1] << endl;
    } else if (loopPlay == false){
    counterPlay = 0;
    }
}

