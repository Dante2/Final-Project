/* MFCC to Wekinator example by Rebecca Fiebrink, 2016
 
 Adapted from Maximilian example code
 
 */

#include "ofApp.h"
#include "maximilian.h"/* include the lib */
#include "time.h"

//-------------------------------------------------------------
ofApp::~ofApp() {
    
}


//----------------------------------------- ---------------------
void ofApp::setup(){
    
    sender.setup(HOST, PORT);
    
    //Load font
    myfont.loadFont("Arial.ttf", 10);
    
    /* some standard setup stuff*/
    ofEnableAlphaBlending();
    ofSetupScreen();
    ofBackground(0, 0, 0);
    ofSetFrameRate(60);
    
    /* This is stuff you always need when you use Maximilian. Don't change it.*/
    sampleRate             = 44100; /* Sampling Rate */
    initialBufferSize    = 512;    /* Buffer Size. you have to fill this buffer with sound*/
    lAudioOut            = new float[initialBufferSize];/* outputs */
    rAudioOut            = new float[initialBufferSize];
    lAudioIn            = new float[initialBufferSize];/* inputs */
    rAudioIn            = new float[initialBufferSize];
    
    
    /* This is a nice safe piece of code */
    memset(lAudioOut, 0, initialBufferSize * sizeof(float));
    memset(rAudioOut, 0, initialBufferSize * sizeof(float));
    memset(lAudioIn, 0, initialBufferSize * sizeof(float));
    memset(rAudioIn, 0, initialBufferSize * sizeof(float));
    
    /* Now you can put anything you would normally put in maximilian's 'setup' method in here. */
    
    mfft.setup(fftSize, 512, 256); //just a forward FFT
    oct.setup(sampleRate, fftSize/2, nAverages);
    mfccs = (double*) malloc(sizeof(double) * 13);
    
    //512 bins, 42 filters, 13 coeffs, min/max freq 20/20000
    mfcc.setup(512, 42, 13, 20, 20000, sampleRate);
    
    ofxMaxiSettings::setup(sampleRate, 2, initialBufferSize);
    ofSoundStreamSetup(2,2, this, sampleRate, initialBufferSize, 4);/* Call this last ! */
    
    
    
    ofSetVerticalSync(true);
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetColor(255, 255, 255,255);
    
    ofSetColor(0, 255, 0, 255);
    myfont.drawString("Sending 13 inputs to port 6448\nUsing message /wek/inputs", 8, 10);
    
    //Draw MFCC values:
    ofSetColor(0, 255, 0,255);
    float xinc = 500.0 / 13;
    for(int i=0; i < 13; i++) {
        float height = mfccs[i] * 200.0;
        ofRect(100 + (i*xinc),200 - height,30, height);
    }
    
    //Send OSC:
    ofxOscMessage m;
    m.setAddress("/wek/inputs");
    for (int i = 0; i < 13; i++) {
        m.addFloatArg(mfccs[i]);
        cout << "mfcc " << i << " = " << mfccs[i] << endl;
    }
    sender.sendMessage(m);
}

//--------------------------------------------------------------
void ofApp::audioRequested     (float * output, int bufferSize, int nChannels){
    //    static double tm;
    for (int i = 0; i < bufferSize; i++){
        wave = lAudioIn[i];
        
        //Calculate the mfccs
        if (mfft.process(wave)) { //is the window buffer full?
            mfft.magsToDB();
            oct.calculate(mfft.magnitudesDB);
            mfcc.mfcc(mfft.magnitudes, mfccs);
        }
        
        memset(lAudioOut, 0, initialBufferSize * sizeof(float));
        memset(rAudioOut, 0, initialBufferSize * sizeof(float));
    }
}

//--------------------------------------------------------------
void ofApp::audioReceived     (float * input, int bufferSize, int nChannels){
    for (int i = 0; i < bufferSize; i++){
        
        /* grab the data out of the arrays*/
        lAudioIn[i] = input[i*2];
        rAudioIn[i] = input[i*2+1];
    }
}

/* 'audioRequested' and 'audioReceived' have both been changed to 'audioIn' and 'audioOut'. Also the project is currently set up without the channels for audio output. The synth I have built for AVP is set up with that and so when these sets of code et integrated the project shall contain those methods */

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

