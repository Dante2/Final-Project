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
    int counter;
    bool playMe;
    float volume;
    
    maxiEnv ADSR[6];
    int currentCount, voice;
    double pitch[6];
    maxiOsc VCO1, VCO2, VCO3[6], VCO4[6], myCounter, LFO1, LFO2, LFO3[6];
    maxiFilter VCF1, VCF2, VCF3[6];
    
public:
    
    maxiClock synthClock;
    double synthMix;
    double VCO1out, VCO2out, VCO3out[6], VCO4out[6], LFO1out, LFO2out, LFO3out[6];
    double VCF1out, VCF2out, VCF3out[6];
    double ADSRout[6];
    //double mySynthOutput[512];
    double mySynthOutput;
    
    // double polySynth(int ticks, int tempo, int index, double signal[], bool play);
    double polySynth(bool play, double tone, float vol);
};

#endif /* synth_hpp */
