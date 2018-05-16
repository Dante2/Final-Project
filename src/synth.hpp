//
//  synth.hpp
//  oscSenderExample
//
//  Created by Jools on 04/05/2018.
//

#ifndef synth_hpp
#define synth_hpp

#include <stdio.h>
#include <iostream>
#include "ofxMaxim.h"

class synth  {
    
    int ticky;
    int BPM;
    bool playMe;
    float volume;
    
    maxiEnv ADSR[6];
    int currentCount, voice;
    double pitch[10];
    maxiOsc VCO1, VCO2, VCO3[6], VCO4[6], myCounter, LFO1, LFO2, LFO3[6], LFO4[6];
    maxiFilter VCF1, VCF2, VCF3[6], VCF4[6];
    maxiClock synthClock;
    double synthMix;
    double VCO1out, VCO2out, VCO3out[6], VCO4out[6], LFO1out, LFO2out, LFO3out[6];
    double VCF1out, VCF2out, VCF3out[6];
    double ADSRout[6];
    
public:

    double mySynthOutput;
    double polySynth(bool play, float vol, int A, int D, int S, int R, int ticks, int tempo, int voices);
};

#endif /* synth_hpp */
