//
//  synth.cpp
//  oscSenderExample
//
//  Created by Jools on 04/05/2018.
//

#include "synth.hpp"

using std::cout;
using std::endl;
using std::cin;
using std::boolalpha;

double synth::polySynth(bool play, double tone, float vol){
    
//    ticky = ticks;
//    BPM = tempo;
//    counter = index;
    playMe = play;
    volume = vol;
    pitch = tone;

    if (playMe == true){
        //cout << boolalpha << "synth playing = " << playMe <<endl;
    // setup synth
    synthClock.setTicksPerBeat(4);
    synthClock.setTempo(90);

        for (int i = 0; i < 6; i++) {
            ADSR[i].setAttack(200);
            ADSR[i].setDecay(200);
            ADSR[i].setSustain(50);
            ADSR[i].setRelease(30);
        }

        synthMix = 0;

        synthClock.ticker();
        if (synthClock.tick) {
            if (voice == 6) {
                voice = 0;
            }

            ADSR[voice].trigger = 1;
            pitch[voice] = voice + 1;
            voice ++;
            // cout << "voice = " << voice <<endl;
        }
    
    //--- oscillators ---//
        
        for (int i = 0; i < 6; i++) {
            ADSRout[i] = ADSR[i].adsr(1., ADSR[i].trigger);
            LFO3out[i] = LFO3[i].sinebuf(0.2);
            VCO3out[i] = VCO3[i].pulse(55 * pitch[i], 0.6);
            VCO4out[i] = VCO4[i].pulse((110 * pitch[i]) + LFO3out[i], 0.2);
            VCF3out[i] = VCF3[i].lores((VCO3out[i] + VCO4out[i]) * 0.5, 250 + ((pitch[i] + LFO3out[i]) * 1000), 10);
            synthMix += VCF3out[i] * ADSRout[i] / 6;
        }
        
        // reset ADSR
        for (int i=0; i < 6; i++) {
            ADSR[i].trigger = 0;
            
            // cout << "synthMix = " << synthMix <<endl;
            mySynthOutput = synthMix * volume;
        }
    }
}
