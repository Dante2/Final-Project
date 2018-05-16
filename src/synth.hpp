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
    double a;
    double d;
    double s;
    double r;
    
    maxiEnv ADSR[500];
    int currentCount, voice, voicey;
    double pitch[500];
    maxiOsc VCO1, VCO2, VCO3[500], VCO4[500], myCounter, LFO1, LFO2, LFO3[500], LFO4[500];
    maxiFilter VCF1, VCF2, VCF3[500], VCF4[500];
    maxiClock synthClock;
    double synthMix;
    double VCO1out, VCO2out, VCO3out[500], VCO4out[500], LFO1out, LFO2out, LFO3out[500];
    double VCF1out, VCF2out, VCF3out[500];
    double ADSRout[500];
    bool changeUp;
    
public:

    double mySynthOutput;
    double polySynth(bool play, float vol, double A, double D, double S, double R, int ticks, int tempo, int voices, bool vChange);
};

#endif /* synth_hpp */
