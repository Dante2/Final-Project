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
    
//    // maxiclock
//    myClock.setTicksPerBeat(4);
//    myClock.setTempo(90);
    
    /* outputs */
    lAudioOut = new float[initialBufferSize];
    rAudioOut = new float[initialBufferSize];
    
    /* inputs */
    lAudioIn = new float[initialBufferSize];
    rAudioIn = new float[initialBufferSize];
    // synthIn = new float[initialBufferSize];
    
    
    // memset?
    /* This is a nice safe piece of code */
    memset(lAudioOut, 0, initialBufferSize * sizeof(float));
    memset(rAudioOut, 0, initialBufferSize * sizeof(float));
    
    memset(lAudioIn, 0, initialBufferSize * sizeof(float));
    memset(rAudioIn, 0, initialBufferSize * sizeof(float));
    
    // ------- FFT ------- //
    
    fftSize = 1024;
    mfft.setup(fftSize, 512, 256);
    nAverages = 12;
    oct.setup(sampleRate, fftSize/2, nAverages);
    
    // ------ SET UP CONVOLVER ------ //
    convolve1.setUp(sampleRate, nAverages, fftSize);
    convolve2.setUp(sampleRate, nAverages, fftSize);
    convolve3.setUp(sampleRate, nAverages, fftSize);
    convolve4.setUp(sampleRate, nAverages, fftSize);
    
    // ------ MFCC ------ //
    
    mfccs = (double*) malloc(sizeof(double) * 13);
    mfcc.setup(512, 42, 13, 20, 20000, sampleRate);
    std::cout << mfccs << endl;
    
    ofxMaxiSettings::setup(sampleRate, 2, initialBufferSize);
    
    /* ofSoundstream and audioStream both refer to ofSoundstream objects and as far as I know should be called at the end of all of this however I have found that in order to get this all working I have had to call it twice in the way that I am doing here */
    ofSoundStreamSetup(2, 2, this, sampleRate, initialBufferSize, 4);/* Call this last ! */
    
    // ------ GUI STUFF ------ //
    gui.setup();
    gui.add(fftToggle.setup("FFT bin magnitudes (pitch/timbre/volume) (512 inputs)", true));
    gui.add(mfccToggle.setup("MFCCs (timbre/vocal) (13 inputs)", true));
    gui.add(rmsToggle.setup("RMS (volume) (13 input)", false));
    
    bHide = true;
    
    ofSetVerticalSync(true);
    
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
    
    // ----- OSC stuff ----- //
    
    destination = "localhost";
    
    // send OSC
    sendPort = 6450;
    sendPortActivate = 6452;
    sendPortDeactivate = 6453;
    
    sender.setup(destination, sendPort);
    senderActivation.setup(destination, sendPortActivate);
    senderDeactivation.setup(destination, sendPortDeactivate);
    
    // receive OSC
    recvPort = 12000;
    recvRawMyo = 5431;
    receiver.setup(recvPort);
    receiver.setup(recvRawMyo);
    
    //------ myo ------//
    
//    myo.setup();
    
    // ----- variable filter ------ //
    
//    myVarFilter.setCutoff(1000);
//    myVarFilter.setResonance(0.2);
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
        
//        if (RMS > 3){
        if (RMS > 0.5){
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
    
    // ofxOscMessage p, q, r;
    
    receiver.getNextMessage(&m);
    
    // Receiveing gesture output messages
    
    // get class 1 for activation
    if(m.getAddress() == "/output_1"){
        messages = m.getAddress();
        cout << "message = " << messages << endl;
        
    // get class 2 for activation
    } else if (m.getAddress() == "/output_2"){
        messages = m.getAddress();
        cout << "message = " << messages << endl;
    } 
    
    // ----- raw Myo data ----- //
    
    
    while(receiver.hasWaitingMessages()) {
        ofxOscMessage p;
        receiver.getNextMessage(&p);

        if(p.getAddress() == "/myo1/emg/scaled/abs/mav/mavg"){
            emg = p.getArgAsFloat(0);
            cout << "emg = " << emg << endl;
        }
        
        // can't seem to get anyq gyro data through. I reckon this is down to Myo Mapper putting out the wrong OSC message for this selection.
        if(p.getAddress() == "/myo1/gyro/fod"){
            gyro = p.getArgAsFloat(0);
            cout << "gyro = " << gyro << endl;
        }
        
        if(p.getAddress() == "/myo1/orientation/quaternion"){
            quaternion = p.getArgAsFloat(0);
            cout << "quaternion = " << quaternion << endl;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    float horizWidth = 500.;
    float horizOffset = 100;
    float fftTop = 250;
    float mfccTop = 350;
    //float chromagramTop = 450;
    
    ofSetColor(255, 0, 0,255);
    
    //FFT magnitudes:
    float xinc = horizWidth / fftSize * 2.0;
    for(int i=0; i < fftSize / 2; i++) {
        float height = mfft.magnitudes[i] * 100;
        ofRect(horizOffset + (i*xinc),250 - height,2, height);
    }
    // myFont.drawString("FFT:", 30, 250);
    
    //MFCCs:
    ofSetColor(0, 255, 0,200);
    xinc = horizWidth / 13;
    for(int i = 0; i < 13; i++) {
        float height = mfccs[i] * 100.0;
        ofRect(horizOffset + (i*xinc), mfccTop - height, 40, height);
                // cout << mfccs[i] << ",";
    }
    
    char rmsString[255]; // an array of chars
    
   // sprintf(rmsString, "RMS: %.2f", RMS);
    //myFont.drawString(rmsString, 12, chromagramTop + 110);
    
    int numInputs = 0;
    
    if (fftToggle) {
        numInputs += fftSize/2;
    }

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

    for (int i = 0; i < bufferSize; i++){

        //------ FFT AND MFCC CALCULATION ON LIVE AUDIO ------//
        
        wave = lAudioIn[i];
        
        if (mfft.process(wave)) {

        int bins   = fftSize / 2.0;
        //do some manipulation
        // int hpCutoff = floor(((mouseX + ofGetWindowPositionX()) / (float) ofGetScreenWidth()) * fftSize / 2.0);
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

        //-------- SYNTH --------//
        
//        LFO1.sinebuf(100 * ofGetMouseY())

        // key t
        // change number of voices in synths
//        voiceControl = 1;
//        if (voiceChange1){
//            voiceChange = voiceChange1;
//            voiceControl * 1.5;
//        } else if (voiceChange1 == false) {
//            voiceChange = voiceChange1;
//            voiceControl = 1;
//        }
//
//        if (voiceChange2){
//            voiceChange = voiceChange2;
//            voiceControl * 3;
//        } else if (voiceChange2 == false) {
//            voiceControl = 1;
//            voiceChange = voiceChange2;
//        }

        voiceControl = 1;
        if (voiceChange1){
            voiceChange = voiceChange1;
            voiceControl * 1.5;
        } else if (voiceChange1 == false) {
            voiceChange = voiceChange1;
            voiceControl = 1;
        }
        
//        } else
        
//            if (voiceChange2){
//            voiceChange = voiceChange2;
//            voiceControl * 0.5;

            
//        } else if (voiceChange2 == false) {
//            voiceControl = 1;
//            voiceChange = voiceChange2;
//        }
        
        
        // synth object takes arguments boolean for playback, float for volume, ints for ADSR respectvely, int for ticks, int for tempo, int for voices
        synth1.polySynth(playSynth, 0.2, 200, 200, 50, 160, 2, 60, 6 * voiceControl, voiceChange);
        synth2.polySynth(playSynth, 0.2, 100, 50, 100, 450, 3, 60, 4 * voiceControl, voiceChange);
        synth3.polySynth(playSynth, 0.2, 500, 60, 300, 3000, 1, 60, 3 * voiceControl, voiceChange);
        synth4.polySynth(playSynth, 0.2, 50, 30, 500, 750, 2, 90, 4 * voiceControl, voiceChange);
        synth5.polySynth(playSynth, 0.8, 20, 60, 750, 100, 4, 120, 6 * voiceControl, voiceChange);
        
        //-------- LOOPER --------//
        
        // record main loop. The length of this sets the length of all other loops
        loop1.recordLoop(44100 * 30, i, inOut, recordNow1);
        
        // slave loops. Length of which is set dynamically by first loop
        loop2.recordLoop(44100 * 30, i, inOut, recordNow2, loop1.loopTracker);
        loop3.recordLoop(44100 * 30, i, inOut, recordNow3, loop1.loopTracker);
        loop4.recordLoop(44100 * 30, i, inOut, recordNow4, loop1.loopTracker);
        loop5.recordLoop(44100 * 30, i, inOut, recordNow5, loop1.loopTracker);
        
        // play loop
        loop1.playLoop(loop1.playIt, 0.8);
        loop2.playLoop(loop1.playIt, 0.8);
        loop3.playLoop(loop1.playIt, 0.8);
        loop4.playLoop(loop1.playIt, 0.8);
        loop5.playLoop(loop1.playIt, 0.8);
        
        // convolvers
        
        // convolve 1
        // live input with synth. wave is already set to live audio in
//        wave2 = synth1.mySynthOutput;
//        convolve1.convolving(convolvePlay1, wave, wave2);
        
        //convolve 2
        // loops with synth
        wave1 = synth1.mySynthOutput;
        wave2 = synth2.mySynthOutput;
        wave3 = synth3.mySynthOutput;
        wave4 = synth4.mySynthOutput;
        wave5 = synth5.mySynthOutput;
        double loopConvolve1 = loop1.myLoopOutput[i];
        double loopConvolve2 = loop2.myLoopOutput[i];
        double loopConvolve3 = loop3.myLoopOutput[i];
        double loopConvolve4 = loop4.myLoopOutput[i];
        double loopConvolve5 = loop5.myLoopOutput[i];
        
        convolve1.convolving(convolvePlay1, loopConvolve1, wave1);
        convolve2.convolving(convolvePlay2, loopConvolve2, wave2);
        convolve3.convolving(convolvePlay3, loopConvolve3, wave3);
        convolve4.convolving(convolvePlay4, loopConvolve4, wave4);
        convolve5.convolving(convolvePlay5, loopConvolve5, wave5);
        
        // ------ OUTPUTS ------- //
        
        // live in
        float ampOut = 0.3;
        output[i * nChannels    ] = guitar.dl(inOut[i], 7000, 0.6, 7) * ampOut;
        output[i * nChannels + 1] = guitar.dl(inOut[i], 7000, 0.6, 7) * ampOut;
        
        // live in and loops
        // key e
        loopsOut = true;
        if (loopsOut){
        float ampOut1 = 0.5;
        output[i * nChannels    ] = guitar.dl(inOut[i],13000,0.7) + loop1.myLoopOutput[i] + loop2.myLoopOutput[i] + loop3.myLoopOutput[i] + loop4.myLoopOutput[i] + loop5.myLoopOutput[i] * ampOut1;
            
        output[i * nChannels + 1] = guitar.dl(inOut[i],13000,0.7) +  loop1.myLoopOutput[i] + loop2.myLoopOutput[i] + loop3.myLoopOutput[i] + loop4.myLoopOutput[i] + loop5.myLoopOutput[i] * ampOut1;
        }
        
        // loop / live in / synth
        // key c
        if (allBasic){
        float ampOut2 = 0.1;
        output[i * nChannels    ] = guitar.dl(inOut[i],13000,0.7) + synth1.mySynthOutput + synth2.mySynthOutput + synth3.mySynthOutput + synth4.mySynthOutput + synth5.mySynthOutput + loop1.myLoopOutput[i] + loop2.myLoopOutput[i] + loop3.myLoopOutput[i] + loop4.myLoopOutput[i] + loop5.myLoopOutput[i] * ampOut2;
            
        output[i * nChannels + 1] = guitar.dl(inOut[i],13000,0.7) + synth1.mySynthOutput + synth2.mySynthOutput + synth3.mySynthOutput + synth4.mySynthOutput + synth5.mySynthOutput + loop1.myLoopOutput[i] + loop2.myLoopOutput[i] + loop3.myLoopOutput[i] + loop4.myLoopOutput[i] + loop5.myLoopOutput[i] * ampOut2;
            
        }

        // convolve
        // key f
        if (convolveOutput){
            float ampOut3 = 0.5;
            output[i * nChannels    ] = convolve1.convolveOut + convolve2.convolveOut + convolve3.convolveOut + convolve4.convolveOut + convolve5.convolveOut * ampOut3;
            output[i * nChannels + 1] = convolve1.convolveOut + convolve2.convolveOut + convolve3.convolveOut + convolve4.convolveOut + convolve5.convolveOut * ampOut3;
        }
        
        //------- ALL STANDARD OUTPUTS -------//

        // all the normal outputs plus SVF taking live audio as input.
        
//        output[i*nChannels    ] = myVarFilter.play(inOut[i], 1, 0, 0, 0) + myFace.dl(inOut[i],13000,0.7) + synth1.mySynthOutput + loop1.myLoopOutput[i] + loop2.myLoopOutput[i] + loop3.myLoopOutput[i] * ampOut;
//
//        output[i*nChannels + 1] = myVarFilter.play(inOut[i], 1, 0, 0, 0) + myFace.dl(inOut[i],13000,0.7) + synth1.mySynthOutput + loop1.myLoopOutput[i] + loop2.myLoopOutput[i] + loop3.myLoopOutput[i] * ampOut;
        
        

        // live audio feeding into SVF with band pass and notch being controlled by mouse X and Y
        // low pass / band pass / high pass / notch
//        output[i * nChannels] = ampOut * myVarFilter.play(inOut[i], 1, ofGetMouseX(), 0, ofGetMouseY());
//
//        output[i * nChannels + 1] = ampOut * myVarFilter.play(inOut[i], 1, ofGetMouseX(), 0, ofGetMouseY());

        
        // SVF with synth and loops as inputs
//        output[i * nChannels] = ampOut * myVarFilter.play(synth1.mySynthOutput, 1, loop1.myLoopOutput[i], 0, ofGetMouseX()) + inOut[i];
//
//        output[i * nChannels + 1] = ampOut * myVarFilter.play(synth1.mySynthOutput, 1, loop1.myLoopOutput[i], 0, ofGetMouseY()) + inOut[i];
    
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
    
    // loop 1 record and play
    if (key == 'q'){
        recordNow1 = true;
    }

    // loop 2
    if (key == 'a'){
        recordNow2 = true;
    }
    
    // loop 3
    if (key == 'z'){
        recordNow3 = true;
    }
    
    //loop 4
    if (key == 'w'){
        recordNow4 = true;
    }
    
    //loop 5
    if (key == 's'){
        recordNow5 = true;
    }
    
//    if (key == 'x'){
//        playLoopNow3 = true;
//    }
    
    // play all loops
//
//        playLoopNow1 = true;
//        playLoopNow2 = true;
//        playLoopNow3 = true;
//        loopsOut = true;
//    }

//    // play synth
//    if (key == 'd'){
//        playSynth = true;
//    }
    
    // play everything
    if (key == 'c'){
        playLoopNow1 = true;
        playLoopNow2 = true;
        playLoopNow3 = true;
        playSynth = true;
        allBasic = true;
    }
    
    // convolvers
    
    // convolve 1
//    if (key == 'r'){
//        playSynth = true;
//        convolvePlay1 = true;
//    } else {
//        playSynth = false;
//        convolvePlay1 = false;
//    }
    
    // convolve 2
    if (key == 'f'){
        playLoopNow1 = true;
        playLoopNow2 = true;
        playLoopNow3 = true;
        convolvePlay1 = true;
        convolvePlay2 = true;
        convolvePlay3 = true;
        playSynth = true;
        convolveOutput = true;
        loopsOut = false;
    }
    
    // synth voice
    if (key == 't'){
        voiceChange1 = true;
    }
    
    if (key == 'g'){
        voiceChange2 = true;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    // loop 1
    if (key == 'q'){
        recordNow1 = false;
    }
    
    // loop 2
    if (key == 'a'){
        recordNow2 = false;
    }
    
    // loop 3
    if (key == 'z'){
        recordNow3 = false;
    }
    
    // loop 4
    if (key == 'w'){
        recordNow4 = false;
    }
    
    // loop 5
    if (key == 's'){
        recordNow5 = false;
    }
    
//    if (key == 'x'){
//        playLoopNow3 = false;
//    }
    
    // all loops
//    if (key == 'e'){
//        playLoopNow1 = false;
//        playLoopNow2 = false;
//        playLoopNow3 = false;
//        loopsOut = false;
//    }
    
//    // stop synth
//    if (key == 'd'){
//        playSynth = false;
//    }
    
    // everything
    if (key == 'c'){
        playLoopNow1 = false;
        playLoopNow2 = false;
        playLoopNow3 = false;
        playSynth = false;
        allBasic = false;
    }
    
    // convolvers
    
    // convolve 1
//    if (key == 'r'){
//        convolvePlay1 = false;
//        playSynth = false;
//    }
    
    // convolve 2
    if (key == 'f'){
        playLoopNow1 = false;
        playLoopNow2 = false;
        playLoopNow3 = false;
        convolvePlay1 = false;
        convolvePlay2 = false;
        convolvePlay3 = false;
        playSynth = false;
        convolveOutput = false;
        loopsOut = true;
    }
    
    // synth voice control
    if (key == 't'){
        voiceChange1 = false;
    }
    if (key == 'g'){
        voiceChange2 = false;
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

