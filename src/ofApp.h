#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxGui.h"
#include "ofxOsc.h"

#include "maxiMFCC.h"
#define HOST "localhost"
//// send on port 6448
//#define PORT 6448
//// listen on port 12000
//#define PORT2 12000

class ofApp : public ofBaseApp{
    
public:
    ~ofApp();/* deconsructor is very useful */
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
    
    void audioRequested     (float * input, int bufferSize, int nChannels); /* output method */
    void audioReceived     (float * input, int bufferSize, int nChannels); /* input method */
    
    float     * lAudioOut; /* outputs */
    float   * rAudioOut;
    
    float * lAudioIn; /* inputs */
    float * rAudioIn;
    
    int        initialBufferSize; /* buffer size */
    int        sampleRate;
    
    int current_msg_string;
    
    //MAXIMILIAN STUFF:
    double wave,sample,outputs[2], ifftVal;
    maxiMix mymix;
    maxiOsc osc;
    
    ofxMaxiFFTOctaveAnalyzer oct;
    int nAverages;
    float *ifftOutput;
    int ifftSize;
    
    float peakFreq = 0;
    float centroid = 0;
    float RMS = 0;
    
    ofxMaxiIFFT ifft;
    ofxMaxiFFT mfft;
    int fftSize;
    int bins, dataSize;
    
    maxiMFCC mfcc;
    double *mfccs;
    
    maxiSample samp;
    
    //GUI STUFF
    bool bHide;
    
    ofxToggle mfccToggle;
    ofxToggle fftToggle;
    ofxToggle chromagramToggle;
    ofxToggle peakFrequencyToggle;
    ofxToggle centroidToggle;
    ofxToggle rmsToggle;
    
    ofxPanel gui;
    
    //ofTrueTypeFont myFont, myFont2;
    

    ofSoundStream audioStream;
    
    
//    string msg_strings[NUM_MSG_STRINGS];
//    float timers[NUM_MSG_STRINGS];
    
    
    
    // Client side:
    
    ofxOscSender sender; // all-important ofxOscSender object
    string destination; // IP address we're sending to
    int sendPort; // port we're sending to
    string sent; // what we're going to send: some stuff you typed
    
    ofxOscReceiver receiver; // OSC receiver
    int recvPort; // port where we listen for messages
    string messages; // string containing the received messages for display
    
};

#endif

