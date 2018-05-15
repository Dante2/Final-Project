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

//double looper::recordLoop(int loopLength, int audioIndex, double signal[], bool recording){
//    index = audioIndex;
//    // cout << "record index = " << index << endl;
//    loopLengthInSamples = loopLength;
//    loopRecord = recording;
//    if (loopRecord == true) {
//        counterRecord ++;
//        // cout << "loop counter = " << counterRecord << endl;
//        if (counterRecord > loopLengthInSamples) {
//            counterRecord = 0;
//        }
//        myLoop[counterRecord] = signal[index];
//        myLoop[counterRecord + 1] = signal[index + 1];
//        //        cout << "loop recorded L = " << myLoop[counterRecord] << endl;
//        //        cout << "loop recorded R = " << myLoop[counterRecord + 1] << endl;
//    } else if (loopRecord == false){
//        counterRecord = 0;
//    }
//}
//
////-------- PLAY LOOP --------//
//
//double looper::playLoop(bool playing, double vol){
//    volume = vol;
//    loopPlay = playing;
//    if (loopPlay == true){
//        counterPlay ++;
//        if (counterPlay > loopLengthInSamples) {
//            counterPlay = 0;
//        }
//        myLoopOutput[index] = myLoop[counterPlay] * volume;
//        // cout << "loop playing = " << myLoopOutput[counterPlay] << endl;
//    } else if (loopPlay == false){
//        counterPlay = 0;
//    }
//}


double looper::recordLoop(int loopLength, int audioIndex, double signal[], bool recording){
    index = audioIndex;
    loopLengthInSamples = loopLength;
    loopRecord = recording;
    if (loopRecord == true) {
        counterRecord ++;
//        cout << "counterRecord = " << counterRecord << endl;
//        if (counterRecord ++){
            for (int i = 0; i < 44100; i++){
                loopTracker = i;
                cout << "loop tracker = " << loopTracker << endl;
                if (loopTracker == 44099){
                    loopTracker2 ++;
                    cout << "loop tracker 2 post = " << loopTracker2 << endl;
//                }
                }
            }
        if (counterRecord > loopLengthInSamples) {
            counterRecord = 0;
        }
        myLoop[counterRecord] = signal[index];
        myLoop[counterRecord + 1] = signal[index + 1];
    } else if (loopRecord == false){
        counterRecord = 0;
        loopTracker2 = 0;
    }
}

//-------- PLAY LOOP --------//

double looper::playLoop(bool playing, double vol){
    volume = vol;
    loopPlay = playing;
    while (loopPlay == true){
//        cout << boolalpha << "looPlay = " << loopPlay << endl;
        counterPlay ++;
        cout << "counterPlay = " << counterPlay << endl;
        
//        if (counterPlay < loopTracker2) {
//        counterPlay ++;
        
//            }
//
        if (counterPlay > loopTracker2) {
            counterPlay = 0;
        }

        myLoopOutput[index] = myLoop[counterPlay] * volume;
        myLoopOutput[index + 1] = myLoop[counterPlay] * volume;
    } else if (loopPlay == false){
        counterPlay = 0;
    }
}

//if (currentPosition > loopLength) {
//
//    (play back your stuff.)
//
//} else {
//
//    currentPosition = 0;
//
//}
//
//
//if (currentPosition < loopLength) {qqqqqqqqq
//
//    (play back your stuff.)
//
//} else {
//
//    currentPosition = 0;
//
//}

