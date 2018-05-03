#include "ofApp.h"
/* include the lib */
#include "maximilian.h"
#include "time.h"

//-------------------------------------------------------------

// deconstructor here why?
ofApp::~ofApp() {
    
}
//--------------------------------------------------------------
void ofApp::setup(){

    ofEnableAlphaBlending();
    ofSetupScreen();
    ofBackground(0, 0, 0);
    ofSetFrameRate(60);
    
    /* This is stuff you always need.*/
    
    /* Sampling Rate */
    sampleRate = 44100;
    
    // Buffer Size
    initialBufferSize = 512;
    
    /* outputs */
    lAudioOut = new float[initialBufferSize];
    rAudioOut = new float[initialBufferSize];
    
    vector<float> spitOut (initialBufferSize);
    
    /* inputs */
    lAudioIn = new float[initialBufferSize];
    rAudioIn = new float[initialBufferSize];
    
    // memset?
    /* This is a nice safe piece of code */
    memset(lAudioOut, 0, initialBufferSize * sizeof(float));
    memset(rAudioOut, 0, initialBufferSize * sizeof(float));
    
    memset(lAudioIn, 0, initialBufferSize * sizeof(float));
    memset(rAudioIn, 0, initialBufferSize * sizeof(float));
    
    // ------- FFT ------- //
    
    fftSize = 1024;
    mfft.setup(fftSize, 512, 256);
    ifft.setup(fftSize, 512, 256);
    
    nAverages = 12;
    oct.setup(sampleRate, fftSize/2, nAverages);
    
    // ------ MFCC ------ //
    
    mfccs = (double*) malloc(sizeof(double) * 13);
    mfcc.setup(512, 42, 13, 20, 20000, sampleRate);
    std::cout << mfccs << endl;
    
    ofxMaxiSettings::setup(sampleRate, 2, initialBufferSize);
    
    // ------ HEEELLLPPPPP ----- //
    /* as far as I can tell this sets up Maxim whereas the 'audiostream one sets up the audio for soundcard. However both audiostream and of ofSoundStream both refer to the same thing so a bit confused */
    ofSoundStreamSetup(2, 2, this, sampleRate, initialBufferSize, 4);/* Call this last ! */
    
    // ------ GUI STUFF ------ //
    gui.setup(); // most of the time you don't need a name
    gui.add(mfccToggle.setup("MFCCs (timbre/vocal) (13 inputs)", true));
    gui.add(rmsToggle.setup("RMS (volume) (13 input)", false));
    
    bHide = true;
    
    // ------ audio stream ------ //
    
    /* currently the below sound stream inputs just one channel. Perhaps I need to set up the whole block of code for each channel? */
    
    // list of available devices printed to console
    ofSoundStreamListDevices();
    
    // set which device to use with id frm listDevices
    audioStream.setDeviceID(0);
    
//    cout << "no of input channels = " << audioStream.getNumInputChannels() << endl;
//    cout << "no of output channels = " << audioStream.getNumOutputChannels() << endl;
    
    // what exactly is "this" reffering to here?
    audioStream.setInput(this);
    
    // why has adding "this" to this line resulted in functionality?
    audioStream.setup(this, 0, 1, sampleRate, initialBufferSize, 4);
    
    cout << "no of input channels = " << audioStream.getNumInputChannels() << endl;
    cout << "no of output channels = " << audioStream.getNumOutputChannels() << endl;
    
    // this is outputting no of channels as defined in stream setup
    audioStream.getNumInputChannels();
    // std::cout << "no of input channels = " << audioStream.getNumInputChannels() << endl;
    
    ofSetVerticalSync(true);
    
    // ----- OSC stuff ----- //
    
    destination = "localhost";
    
    //sendPort = 6448;
    sendPort = 6450;
    sendPortActivate = 6452;
    sendPortDeactivate = 6453;
    
    sender.setup(destination, sendPort);
    senderActivation.setup(destination, sendPortActivate);
    senderDeactivation.setup(destination, sendPortDeactivate);
    
    recvPort = 12000;
    receiver.setup(recvPort);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // ----- send OSC ----- //
    
    ofxOscMessage m, n, o;
    bool switchedOn = false;
    if (rmsToggle) {
        m.setAddress("mfccs");
        for (int i = 0; i < 13; i++) {
            m.addFloatArg(mfccs[i]);
        }
        sender.sendMessage(m);
        
        // ------ SWITCH WEKI DTW RECORDING ON AND OFF WHEN SOUND DETECTED ----- //
        
        if (RMS > 3){
            switchedOn = true;
            n.setAddress("/wekinator/control/startDtwRecording");
            senderActivation.sendMessage(n);
            // cout << "message out = " << m.getAddress() << endl;
            
            if (switchedOn == true){
                    switchedOn = false;
                    o.setAddress("/wekinator/control/stopDtwRecording");
                    senderDeactivation.sendMessage(o);
                    switchedOn = false;
                }
            }
        }
    
        // ----- receive OSC ----- //
        receiver.getNextMessage(&m);
    
        // get class 1 for activation
        if(m.getAddress() == "/output_1"){
            messages = m.getAddress();
            
            cout << "message = " << messages << endl;
        
        // get class 2 for activation
        } else if (m.getAddress() == "/output_2"){
            messages = m.getAddress();
            
            cout << "message = " << messages << endl;
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
    
    //FFT magnitudes:
    float xinc = horizWidth / fftSize * 2.0;
//    for(int i=0; i < fftSize / 2; i++) {
//        float height = mfft.magnitudes[i] * 100;
//        ofRect(horizOffset + (i*xinc),250 - height,2, height);
//    }
//    //myFont.drawString("FFT:", 30, 250);
    
    
    //MFCCs:
    ofSetColor(0, 255, 0,200);
    xinc = horizWidth / 13;
    for(int i = 0; i < 13; i++) {
        float height = mfccs[i] * 100.0;
        ofRect(horizOffset + (i*xinc),mfccTop - height,40, height);
                // cout << mfccs[i] << ",";
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
   // sprintf(rmsString, "RMS: %.2f", RMS);
    //myFont.drawString(rmsString, 12, chromagramTop + 110);
    
    int numInputs = 0;

    if (mfccToggle) {
        numInputs += 13;
        //std::cout << "MFCC inputs = " << numInputs << endl;
    }
    
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

}

//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels) {

    // using this audio output method seems to deactivate the mfcc GUI but is currently giving me very distorted sound //
    for (int i = 0; i < bufferSize; i++){
        
        wave = lAudioIn[i];
        //std::cout << "audio = " << wave << endl;
        if (mfft.process(wave)) {

        int bins   = fftSize / 2.0;
        //do some manipulation
        int hpCutoff = floor(((mouseX + ofGetWindowPositionX()) / (float) ofGetScreenWidth()) * fftSize / 2.0);
        //highpass
        //                        memset(mfft.magnitudes, 0, sizeof(float) * hpCutoff);
        //                        memset(mfft.phases, 0, sizeof(float) * hpCutoff);
        //lowpass
//        memset(mfft.magnitudes + hpCutoff, 0, sizeof(float) * (bins - hpCutoff));
//        memset(mfft.phases + hpCutoff, 0, sizeof(float) * (bins - hpCutoff));

        //----- THIS CHROMOGRAM CAN BE USED FOR PITCH RECOGNITION LATER -----//

        
            /* for (int j = 0; j < 12; j++) {
             chromagram[j] = 0;
             }
             int j = 0;
             for (int i = 0; i < oct.nAverages; i++) {
             chromagram[j] += oct.averages[i];
             j++;
             j = j % 12;
             } */

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
            // cout << mfft.spectralFlatness() << ", " << mfft.spectralCentroid() << endl;
        }
        
        //-------- LOOPER --------//
        
        //loop1.triggerLoopRecord(trigger);
//        cout << "trigger = " << loop1.triggerLoopRecord(trigger) << endl;
//        cout << boolalpha << "trigger = " << loop1.triggerLoopRecord(trigger) << endl;
        
        //loop1.recordLoop(88200, inOut, recordNow);
        
        
        loop1.recordLoop(88200, inOut[i], inOut, recordNow);
        loop1.playLoop(inOut[i], playLoopNow);

        // RECORD LOOP
        // record loop when boolean is set to true
//        if (loopRecord == true) {
//
//            // counter for loop recording
//            counterRecord ++;
//            // cout << "counter record = " << counterRecord << endl;
//
//            // if loop recording goes over allotted time set then return to the beginning
//            if (counterRecord > loopLengthInSamples) {
//                counterRecord = 0;
//            }
//
//            // array myLoop holds a recording by storing values taken from the input stream. We write to two versions of the array to retain the interleaving that takes place thus outputting audio to both right and left speaker.
//            myLoop[counterRecord] = inOut[i];
//            myLoop[counterRecord + 1] = inOut[i + 1];
//            // cout << "loopy record = " << counterRecord << endl;
//        }
//
//        // PLAY LOOPER
//        // record loop when boolean is set to true
//        if (loopPlay == true){
//
//            // counter for loop playback
//            counterPlay ++;
//
//            // if loop playback goes over allotted time set then return to the beginning
//            if (counterPlay > loopLengthInSamples) {
//                counterPlay = 0;
//            }
//
//            // array myLoopOutput holds finished loop. The recording held in myLoop is now sent to a dsedicated output for the looper. We write to two versions of the array to retain the interleaving that takes place as is done throughout the program thus outputting audio to both right and left speaker. This output is then added to the main output in the mix.
//            myLoopOutput[i] = myLoop[counterPlay];
//            myLoopOutput[i + 1] = myLoop[counterPlay + 1];
//            // cout << "playhead = " << counterPlay << endl;
//            // cout << "loopy play = " << myLoopOutput << endl;
//        }
        
        float ampOut = 0.7;

        output[i*nChannels    ] = myFace.dl(inOut[i],13000,0.7) * ampOut;
        
        output[i*nChannels + 1] = myFace.dl(inOut[i],13000,0.7) * ampOut;
    }
}

//----------------------------------------------------------------------------

void ofApp::audioReceived     (float * input, int bufferSize, int nChannels){
    
    float sum = 0;
    for (int i = 0; i < bufferSize; i++){
        
        lAudioIn[i] = input[i*2];
        rAudioIn[i] = input[i*2+1];
        
        // our array for jacking in our live audio input stream
        inOut[i] = input[i*2];
        
        // sum is done for our stereo output
        sum += input[i*2] * input[i*2];

    }
    RMS = sqrt(sum);
    // cout << " Raw RMS = " << RMS << endl;
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == 's'){
        recordNow = true;
    } else {
        recordNow = false;
    }
                                     
    if (key == 'd'){
        playLoopNow = true;
    } else {
        playLoopNow = false;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    if (key == 's'){
        recordNow = false;
    }

    if (key == 'd'){
        playLoopNow = false;
    }
    
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

