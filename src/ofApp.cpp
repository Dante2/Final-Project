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
    
    // ------- send OSC -------- //
    
    // Wekinator
    sendPort = 6450;
    sender.setup(destination, sendPort);
    
    // DTW
    sendPortActivate = 6452;
    sendPortDeactivate = 6453;
    senderActivation.setup(destination, sendPortActivate);
    senderDeactivation.setup(destination, sendPortDeactivate);
    
    // KNN
    actKNN = 3000;
    deActKNN = 4000;
    sendActKNN.setup(destination, actKNN);
    sendDeActKNN.setup(destination, deActKNN);
    
    // ------ receive OSC ------ //
    
    // recieve wekinator
    recvPort = 12000;
    receiver1.setup(recvPort);
    
    // recieve myo mapper
    recvRawMyo = 5431;
    receiver2.setup(recvRawMyo);
    
    // recieve pedal
    recvPedal = 15000;
    receiverPedal.setup(recvPedal);
    
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
        
        // ------ SWITCH WEKI KNN CLASSIFIER RECORDING ON AND OFF WHEN SOUND DETECTED ----- //
        
        ofxOscMessage a, b;
        if (RMS > 0.2){
            switchedOn = true;
            a.setAddress("/wekinator/control/startRecording");
            sendActKNN.sendMessage(a);
            // cout << "message out = " << m.getAddress() << endl;
            
            if (switchedOn == true){
                switchedOn = false;
                b.setAddress("/wekinator/control/stopRecording");
                sendDeActKNN.sendMessage(b);
                switchedOn = false;
            }
        }
        
        
        // ------ SWITCH WEKI DTW RECORDING ON AND OFF WHEN SOUND DETECTED ----- //
        
        if (RMS > 0.2){
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
    
    // ------ Pedal ------- //
    
    while(receiverPedal.hasWaitingMessages()){
        ofxOscMessage z;
        int pedal;
        receiverPedal.getNextMessage(&z);
        if(z.getAddress() == "/pedal"){
            cout << "pedal = " << z.getAddress() << endl;
            
            pedal = z.getArgAsInt(0);
            cout << "pedal = " << pedal << endl;
            
            if (pedal == 1){
                recordNow1 = true;
            }
            
            if (pedal == 6){
                recordNow1 = false;
            }
            
            if (pedal == 2){
                recordNow2 = true;
            }
            
            if (pedal == 7){
                recordNow2 = false;
            }

            if (pedal == 3){
                recordNow3 = true;
            }
            
            if (pedal == 8){
                recordNow3 = false;
            }
            
            if (pedal == 4){
                rmsToggle = true;
            }

        }
    }

    
    
    
    while(receiver1.hasWaitingMessages()){
        ofxOscMessage q;
        receiver1.getNextMessage(&q);
        
        bool convolveOn;
        
        if(q.getAddress() == "/output_1"){
            messages = q.getAddress();
            cout << "message = " << messages << endl;
            
            rmsToggle = false;
            allBasic = false;
            convolveOn = true;
//            playLoopNow1 = true;
//            playLoopNow2 = true;
//            playLoopNow3 = true;
            convolvePlay1 = true;
            convolvePlay2 = true;
            convolvePlay3 = true;
            playSynth = true;
            convolveOutput = true;
            
            
            // get class 2 for activation
        }
        
        if (convolveOn == true){
            if (q.getAddress() == "/output_2"){
                messages = q.getAddress();

                rmsToggle = false;
                
                voiceChange1 = true;
                
                ADSRcontrol = true;
                
                tempoControl = true;
                
                ticksControl = true;
                }
                
                cout << "message = " << messages << endl;
            
            // get class 3 for activation
        } else if (q.getAddress() == "/output_3"){
            messages = q.getAddress();
            
            convolveOutput = false;
            convolveOn = false;
            allBasic = true;
            
            cout << "message = " << messages << endl;
            
//            playSynth = false;
//            allBasic = false;
            
        }
    }
    // Receiveing gesture output messages
    
    // get class 1 for activation
    
    // ----- raw Myo data ----- //
    
    while(receiver2.hasWaitingMessages()) {
        ofxOscMessage p;
        receiver2.getNextMessage(&p);
        //        cout << boolalpha << "receiver 2 update = " << receiver2.getNextMessage(&p)<< endl;
        
        if(p.getAddress() == "/myo1/emg/scaled/abs/mav/mavg"){
            emg = p.getArgAsFloat(0);
        }
        
        // can't seem to get any gyro data through. I reckon this is down to Myo Mapper putting out the wrong OSC message for this selection.
        if(p.getAddress() == "/myo1/gyro/fod"){
            gyro = p.getArgAsFloat(0);
        }
        
        if(p.getAddress() == "/myo1/orientation/quaternion"){
            quaternion = p.getArgAsFloat(0);
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
            
         int hpCutoff = floor(((mouseX + ofGetWindowPositionX()) / (float) ofGetScreenWidth()) * fftSize / 2.0);
            
        //highpass
        //                        memset(mfft.magnitudes, 0, sizeof(float) * hpCutoff);
        //                        memset(mfft.phases, 0, sizeof(float) * hpCutoff);
            
            
        //lowpass
        memset(mfft.magnitudes + hpCutoff, 0, sizeof(float) * (bins - hpCutoff));
        memset(mfft.phases + hpCutoff, 0, sizeof(float) * (bins - hpCutoff));

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

//        LFO1.sinebuf(100)
        
        // control ADSR
        // control tempo
        // control base frequencies
        // control modulating frequencies
        // control ticks
        
        // voice control
        // key t
        voiceControl = 1;
        if (voiceChange1){
            voiceChange = voiceChange1;
            voiceControl * 1.5;
        } else if (voiceChange1 == false) {
            voiceChange = voiceChange1;
            voiceControl = 1;
        }
        
        // ADSR control
        // key g
        adsrControl = 1;
        float adsrModulate = 1;
        if (ADSRcontrol){
            adsrControl = 0.002 * quaternion;
            adsrModulate = 0.03;
        } else if (ADSRcontrol == false) {
            adsrControl = 1;
            adsrModulate = 1;
        }
        
        // tempo control
        // key b
        BPM = 1;
        int BPMmodulate = 1;
        if (tempoControl){
            BPM = 2;
            BPMmodulate = 0.05;
        } else if (tempoControl == false){
            BPM = 1;
            BPMmodulate = 1;
        }
        
        // ticks control
        // key y
        ticks = 1;
        int tickModulate = 1;
        if (ticksControl){
            ticks = 2;
            tickModulate = 0.05;
        } else if (ticksControl == false){
            ticks = 1;
            tickModulate = 1;
        }
        
        // synth object takes arguments boolean for playback, float for volume, ints for ADSR respectvely, int for ticks, int for tempo, int for voices
        synth1.polySynth(playSynth, 0.2, 200 * adsrControl, 200 * adsrControl, 50 * adsrControl, 160 * adsrControl, 2 * ticks, 60 * BPM, 6 * voiceControl, voiceChange);
        synth2.polySynth(playSynth, 0.5, 100 * adsrControl, 50 * adsrControl, 100 * adsrControl, 450 * adsrControl, 3 * (ticks * tickModulate), 60 * BPM, 4 * voiceControl, voiceChange);
        synth3.polySynth(playSynth, 0.5, 500 * adsrControl, 60 * adsrControl, 300 * adsrControl, 3000 * adsrControl, 1 * ticks, 60 * BPM, 3 * voiceControl, voiceChange);
        synth4.polySynth(playSynth, 0.5, 50 * (adsrControl * adsrModulate), 30  * adsrControl, 500  * (adsrControl * adsrModulate), 750  * (adsrControl * adsrModulate), 2 * ticks, 90 * (BPM * BPMmodulate), 4 * voiceControl, voiceChange);
        synth5.polySynth(playSynth, 0.5, 20 * adsrControl, 60 * adsrControl, 750 * adsrControl, 100 * adsrControl, 4 * (ticks * tickModulate), 120 * (BPM * BPMmodulate), 6 * voiceControl, voiceChange);
    
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
        
        // loop / live in / synth
        
        allBasic = true;
        
        if (allBasic){
            float ampOut2 = 0;
            for (int i = 0; i < 30; i++){
        float ampOut2 = 0.03 * i;
            }
        output[i * nChannels    ] = guitar.dl(inOut[i],13000,0.7) + loop1.myLoopOutput[i] + loop2.myLoopOutput[i] + loop3.myLoopOutput[i] + loop4.myLoopOutput[i] + loop5.myLoopOutput[i] * ampOut2;

        output[i * nChannels + 1] = guitar.dl(inOut[i],13000,0.7) + loop1.myLoopOutput[i] + loop2.myLoopOutput[i] + loop3.myLoopOutput[i] + loop4.myLoopOutput[i] + loop5.myLoopOutput[i] * ampOut2;
        }

        // convolve
        if (convolveOutput){
            float ampOut3 = 0;
            for (int i = 0; i < 30; i++){
                ampOut3 = 0.03 * i;
            }
            output[i * nChannels    ] = convolve1.convolveOut + convolve2.convolveOut + convolve3.convolveOut + convolve4.convolveOut + convolve5.convolveOut * ampOut3;
            output[i * nChannels + 1] = convolve1.convolveOut + convolve2.convolveOut + convolve3.convolveOut + convolve4.convolveOut + convolve5.convolveOut * ampOut3;
        }
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
//    if (key == 'q'){
//        recordNow1 = true;
//    }

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
    
    // ADSR
    if (key == 'g'){
        ADSRcontrol = true;
    }
    
    // tempo
    if (key == 'b'){
        tempoControl = true;
    }
    
    // ticks
    if (key == 'y'){
        ticksControl = true;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    // loop 1
//    if (key == 'q'){
//        recordNow1 = false;
//    }
    
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
    
    // ADSR
    if (key == 'g'){
        ADSRcontrol = false;
    }
    
    // tempo
    if (key == 'b'){
        tempoControl = false;
    }
    
    // ticks
    if (key == 'y'){
        ticksControl = false;
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

