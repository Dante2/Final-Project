#ifndef _TEST_APP
#define _TEST_APP

//#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxGui.h"
#include "ofxOsc.h"
//#include "ofxMyo.h"
#include "maxiMFCC.h"
#include "looper.hpp"
#include "synth.hpp"

class ofApp : public ofBaseApp{
    
public:
    /* deconsructor is very useful */
    ~ofApp();
    void setup();
    void update();
    void draw();
//    void exit();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    
    /* output method */
    void audioRequested (float * output, int bufferSize, int nChannels);
    /* input method */
    void audioReceived (float * input, int bufferSize, int nChannels);
    
    void audioOut(float * output, int bufferSize, int nChannels);
    
    /* outputs */
    float * lAudioOut;
    float * rAudioOut;
    
    // array for passing audio values around
    double inOut[512];
    
    //------ EFFECTS RACK ------//
    // delay ----
    maxiDelayline myFace;
    
    // float * output;
    
    /* inputs */
    float * lAudioIn;
    float * rAudioIn;
    
    float * synthIn;
    
    /* buffer size */
    int initialBufferSize;
    int sampleRate;
    
    //--------- SYNTH ----------//
    maxiClock myClock;
    maxiEnv ADSR[6];
    double synthMix;
    int currentCount, voice;
    double pitch[6];
    maxiOsc VCO1, VCO2, VCO3[6], VCO4[6], myCounter, LFO1, LFO2, LFO3[6];
    double VCO1out, VCO2out, VCO3out[6], VCO4out[6], LFO1out, LFO2out, LFO3out[6];
    double VCF1out, VCF2out, VCF3out[6];
    double ADSRout[6];
    maxiFilter VCF1, VCF2, VCF3[6];
    
    bool playSynth;
    
    synth synth1;
    
    /* Looper */

    // loops
    looper loop1, loop2, loop3;
    
    // triggers for each loops' functionalities
    // loop1
    bool recordNow1;
    bool playLoopNow1;
    
    // loop2
    bool recordNow2;
    bool playLoopNow2;
    
    // loop3
    bool recordNow3;
    bool playLoopNow3;
    
    //MAXIMILIAN STUFF:
    double wave, wave2, sample, outputs[2], ifftVal;
    
    // Pitch recognition??
    ofxMaxiFFTOctaveAnalyzer oct;
    int nAverages;
    float *ifftOutput;
    int ifftSize;
    
    // data retrieval options
    float peakFreq = 0;
    float centroid = 0;
    float RMS = 0;
    
    // FFT
    ofxMaxiIFFT ifft;
    ofxMaxiFFT mfft, mfft2;
    int fftSize;
    int bins, dataSize;
    
    bool convolve;
    double convolveOut;
    
    // MFCC
    maxiMFCC mfcc;
    double *mfccs;
    
    // maxiSample samp;
    
    // GUI STUFF
    bool bHide;
    
    ofxToggle mfccToggle;
    ofxToggle fftToggle;
    ofxToggle chromagramToggle;
    ofxToggle peakFrequencyToggle;
    ofxToggle centroidToggle;
    ofxToggle rmsToggle;
    
    ofxPanel gui;
    
    //ofTrueTypeFont myFont, myFont2;

    // soundstream
    ofSoundStream audioStream;
    
    

    // ----- OSC ------ //
    // Needs to go into a class of its own
    
    // ----- SEND ----- //
    // ofxOscSender object
    ofxOscSender sender, senderActivation, senderDeactivation;
    
    // IP address we're sending to
    string destination;
    // port we're sending to
    int sendPort;
    
    int sendPortActivate;
    
    int sendPortDeactivate;
    // what we're going to send
    string sent;
    
    // ----- RECEIEVE ----- //
    // OSC receiver
    ofxOscReceiver receiver;
    // port where we listen for messages
    int recvPort;
    // string containing the received messages for display
    
    // receive raw Myo data
    int recvRawMyo;
    string messages;
    
    //----- Myo -----//
//    ofxMyo::Myo myo;
    
    // ----- myo mapper ------ //
    float emg = 0;
    float gyro = 0;
    float quaternion = 0;
    
    maxiSVF myVarFilter;
  
};

#endif

