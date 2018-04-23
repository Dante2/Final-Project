#ifndef _TEST_APP
#define _TEST_APP
#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "maxiMFCC.h"

// ------ this has now been set up in app.setup ------ //

//#define HOST "localhost"
//send on port 6448
//#define PORT 6448
//listen on port 12000
//#define PORT2 12000

class ofApp : public ofBaseApp{
    
public:
    /* deconsructor is very useful */
    ~ofApp();
    void setup();
    void update();
    void draw();
    
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
    
    /* inputs */
    float * lAudioIn;
    float * rAudioIn;
    
    /* buffer size */
    int initialBufferSize;
    int sampleRate;
    
    // int current_msg_string;
    
    //MAXIMILIAN STUFF:
    double wave,sample,outputs[2], ifftVal;

    // necessary????
//    maxiMix mymix;
    maxiOsc osc, myCounter, mySwitchableOsc;
    int CurrentCount;
    double myOscOutput;
    
    maxiOsc mySine1;
    double spitOut;
    
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
    ofxMaxiFFT mfft;
    int fftSize;
    int bins, dataSize;
    
    // MFCC
    maxiMFCC mfcc;
    double *mfccs;
    
    maxiSample samp;
    
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
    string messages;
};

#endif

