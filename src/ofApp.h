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
#include "slaveLoop.hpp"
#include "synth.hpp"
#include "convolve.hpp"

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
    maxiDelayline guitar, loops;
    maxiDistortion myArm;
    maxiFlanger myTeeth;
    maxiChorus myHead;
    
    // filter //
//    maxiSVF myVarFilter;
    
    // ---------- //
    
    /* inputs */
    float * lAudioIn;
    float * rAudioIn;
    
    /* buffer size */
    int initialBufferSize;
    int sampleRate;
    
    //--------- SYNTH ----------//
//    maxiClock myClock;
    maxiFilter VCF1, VCF2, VCF3[6];

    maxiOsc LFO1;
    bool voiceChange, voiceChange1, voiceChange2;
    float voiceControl;
    
    synth synth1, synth2, synth3, synth4, synth5;
    bool playSynth;
    
    /* Looper */

    // loops
    looper loop1;
    slaveLoop loop2, loop3, loop4, loop5;
    
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
    
    // loop4
    bool recordNow4;
    bool playLoopNow4;
    
    // loop5
    bool recordNow5;
    bool playLoopNow5;
    
    //MAXIMILIAN STUFF:
    double wave, wave1, wave2, wave3, wave4, wave5, sample, outputs[2], ifftVal;
    
    // FFT
    ofxMaxiFFTOctaveAnalyzer oct;
    int nAverages;
    float * ifftOutput;
    int ifftSize;
    ofxMaxiFFT mfft;
    int fftSize;
    int bins, dataSize;
//    ofxMaxiIFFT ifft;
    
    // data retrieval options
    float peakFreq = 0;
    float centroid = 0;
    float RMS = 0;
    
    // convolvers
    convolve convolve1, convolve2, convolve3, convolve4, convolve5;
    bool convolvePlay1, convolvePlay2, convolvePlay3, convolvePlay4, convolvePlay5;
    
    // MFCC
    maxiMFCC mfcc;
    double *mfccs;
    
    // GUI STUFF
    bool bHide;
    
    ofxToggle mfccToggle;
    ofxToggle fftToggle;
    ofxToggle chromagramToggle;
    ofxToggle peakFrequencyToggle;
    ofxToggle centroidToggle;
    ofxToggle rmsToggle;
    
    ofxPanel gui;

    // soundstream
    ofSoundStream audioStream;
    
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
    
    // ------ output controls ------ //
    // bool liveIn = false;
    bool allBasic = false;
    bool convolveOutput = false;
    bool loopsOut;
    
};

#endif

