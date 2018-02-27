#include "ofApp.h"
#include "maximilian.h"/* include the lib */
#include "time.h"

//-------------------------------------------------------------

// deconstructor here why?
ofApp::~ofApp() {
    
}

//--------------------------------------------------------------
void ofApp::setup(){
    
    sender.setup(HOST, PORT);
    receiver.setup(PORT);
    
    ofEnableAlphaBlending();
    ofSetupScreen();
    ofBackground(0, 0, 0);
    ofSetFrameRate(60);
    
    current_msg_string = 0;
    
    /* This is stuff you always need.*/
    
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
    
    fftSize = 1024;
    mfft.setup(fftSize, 512, 256);
    ifft.setup(fftSize, 512, 256);
    
    nAverages = 12;
    oct.setup(sampleRate, fftSize/2, nAverages);
    
    mfccs = (double*) malloc(sizeof(double) * 13);
    mfcc.setup(512, 42, 13, 20, 20000, sampleRate);
    std::cout << mfccs << endl;
    
    ofxMaxiSettings::setup(sampleRate, 2, initialBufferSize);
    
    // is this necessary?
    ofSoundStreamSetup(2, 2, this, sampleRate, initialBufferSize, 4);/* Call this last ! */
    
    //GUI STUFF
    gui.setup(); // most of the time you don't need a name
    gui.add(mfccToggle.setup("MFCCs (timbre/vocal) (13 inputs)", true));
    gui.add(rmsToggle.setup("RMS (volume) (13 input)", false));
    
    bHide = true;
    
    /* currently the below sound stream inputs just one channel. Perhaps I need to set up the whole block of code for each channel? */
    
    // list of available devices printed to console
    ofSoundStreamListDevices();
    
    // set which device to use with id frm listDevices
    audioStream.setDeviceID(0);
    
    // what exactly is "this" reffering to here?
    audioStream.setInput(this);
    
    // why has adding "this" to this line resulted in functionality?
    audioStream.setup(this, 0, 1, sampleRate, initialBufferSize, 4);
    
    // this is outputting no of channels as defined in stream setup
    audioStream.getNumInputChannels();
    std::cout << "no of input channels = " << audioStream.getNumInputChannels() << endl;
    
    ofSetVerticalSync(true);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    ofxOscMessage m;
    
    // receive messages
    // hide old messages
    for(int i = 0; i < NUM_MSG_STRINGS; i++){
        if(timers[i] < ofGetElapsedTimef()){
            msg_strings[i] = "";
            //std::cout << msg_strings[i] << endl;
        }
    }
    
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        // get the next message
        // ofxOscMessage m;
        receiver.getNextMessage(m);
        
        // check for mouse moved message
        if(m.getAddress() == "/mouse/position"){
            // both the arguments are int32's
            mouseX = m.getArgAsInt32(0);
            mouseY = m.getArgAsInt32(1);
        }
//        // check for mouse button message
//        else if(m.getAddress() == "/mouse/button"){
//            // the single argument is a string
//            mouseButtonState = m.getArgAsString(0);
//        }
        
        // add to the list of strings to display
        string msg_string;
        msg_strings[current_msg_string] = msg_string;
        timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
        current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
        // clear the next line
        msg_strings[current_msg_string] = "";
    }
    

    
    // sending messages
    // ofxOscMessage m;
    m.setAddress("/wek/inputs");
    
    if (rmsToggle) {
        if (RMS > 2){
        //ofxOscMessage m;
        m.setAddress("/wek/inputs");
        for (int i = 0; i < 13; i++) {
            m.addFloatArg(RMS);
        }
    sender.sendMessage(m);
    //cout << "rms = " << RMS << endl;
    }
}
    
    else if
        (mfccToggle) {
        // ofxOscMessage m;
        m.setAddress("/wek/inputs");
        for (int i = 0; i < 13; i++) {
            m.addFloatArg(mfccs[i]);
            //cout << "mfccs = " << mfccs[i] << endl;
        }
        sender.sendMessage(m);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    float horizWidth = 500.;
    float horizOffset = 100;
    //float fftTop = 250;
    float mfccTop = 350;
    //float chromagramTop = 450;
    
    ofSetColor(255, 0, 0,255);
    
    string buf;
    buf = "listening for osc messages on port" + ofToString(PORT);
    ofDrawBitmapString(buf, 10, 20);
    
    // draw mouse state
    buf = "mouse: " + ofToString(mouseX, 4) +  " " + ofToString(mouseY, 4);
    ofDrawBitmapString(buf, 430, 20);
    ofDrawBitmapString(mouseButtonState, 580, 20);
    
    for(int i = 0; i < NUM_MSG_STRINGS; i++){
        ofDrawBitmapString(msg_strings[i], 10, 40 + 15 * i);
    }
    
//    //FFT magnitudes:
    float xinc = horizWidth / fftSize * 2.0;
//    for(int i=0; i < fftSize / 2; i++) {
//        float height = mfft.magnitudes[i] * 100;
//        ofRect(horizOffset + (i*xinc),250 - height,2, height);
//    }
//    //myFont.drawString("FFT:", 30, 250);
    
    
    //MFCCs:
    ofSetColor(0, 255, 0,200);
    xinc = horizWidth / 13;
    for(int i=0; i < 13; i++) {
        float height = mfccs[i] * 100.0;
        ofRect(horizOffset + (i*xinc),mfccTop - height,40, height);
        //        cout << mfccs[i] << ",";
    }
    
    ofSetColor(255, 255, 255,255);
//
//    char peakString[255]; // an array of chars
//    sprintf(peakString, "Peak Frequency: %.2f", peakFreq);
//    //myFont.drawString(peakString, 12, chromagramTop + 50);
//
//    char centroidString[255]; // an array of chars
//    sprintf(centroidString, "Spectral Centroid: %f", centroid);
//    //myFont.drawString(centroidString, 12, chromagramTop + 80);
    
    char rmsString[255]; // an array of chars
    sprintf(rmsString, "RMS: %.2f", RMS);
    //myFont.drawString(rmsString, 12, chromagramTop + 110);
    
    int numInputs = 0;

    if (mfccToggle) {
        numInputs += 13;
        //std::cout << "MFCC inputs = " << numInputs << endl;
    }
    
    // rms num inputs seems to be 1 + mfcc inputs. Why?
    if (rmsToggle) {
        numInputs++;
        //std::cout << "rms inputs = " << numInputs << endl;
    }
    
    char numInputsString[255]; // an array of chars
    sprintf(numInputsString, "Sending %d inputs total", numInputs);
    //myFont.drawString(numInputsString, 450, 100);

    if( bHide ){
        gui.draw();
    }
}

//--------------------------------------------------------------
void ofApp::audioRequested     (float * output, int bufferSize, int nChannels){
    for (int i = 0; i < bufferSize; i++){
        wave = lAudioIn[i];
        //std::cout << "audio = " << wave << endl;
        if (mfft.process(wave)) {
            
            mfft.magsToDB();
            oct.calculate(mfft.magnitudesDB);
            
            float sum = 0;
            float maxFreq = 0;
            int maxBin = 0;
            
            for (int i = 0; i < fftSize/2; i++) {
                sum += mfft.magnitudes[i];
                if (mfft.magnitudes[i] > maxFreq) {
                    maxFreq=mfft.magnitudes[i];
                    maxBin = i;
                }
            }
            centroid = sum / (fftSize / 2);
            peakFreq = (float)maxBin/fftSize * 44100;
            mfcc.mfcc(mfft.magnitudes, mfccs);
        }
        
        lAudioOut[i] = 0;
        rAudioOut[i] = 0;
        
    }
}

//--------------------------------------------------------------
void ofApp::audioReceived     (float * input, int bufferSize, int nChannels){
			
    /* You can just grab this input and stick it in a double, then use it above to create output*/
    
    float sum = 0;
    for (int i = 0; i < bufferSize; i++){
        
        /* you can also grab the data out of the arrays*/
        
        lAudioIn[i] = input[i*2];
        rAudioIn[i] = input[i*2+1];
        
        sum += input[i*2] * input[i*2];
        
    }
    RMS = sqrt(sum);
    
}

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

