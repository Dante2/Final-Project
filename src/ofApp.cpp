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
    
    // 8current_msg_string = 0;
    
    /* This is stuff you always need.*/
    
    /* Sampling Rate */
    sampleRate = 44100;
    
    // Buffer Size
    initialBufferSize = 512;
    
    /* outputs */
    lAudioOut = new float[initialBufferSize];
    rAudioOut = new float[initialBufferSize];
    
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

    // currently using 'rms toggle' for switching on recording and the rms threshold for activating the recording mehanism in Weki. Perhaps I can then set the activation of the recording mode with accelrometer gesture (wave of the arm)? This would then switch it on in here or trigger activation of Weki's GUI to set it to record.
    
    ofxOscMessage m, n, o;
    bool switchedOn = false;
    if (rmsToggle) {
        m.setAddress("mfccs");
        for (int i = 0; i < 13; i++) {
            m.addFloatArg(mfccs[i]);
        }
        sender.sendMessage(m);
        
        // ------ SWITCH WEKI DTW RECORDING ON AND OFF WHEN SOUND DETECTED ----- //
        //if (switchedOn == false){
        if (RMS > 3){
            //cout << "RMS On = " << RMS << endl;
            switchedOn = true;
            //cout << "1. OSC Out switch = " << std::boolalpha << switchedOn << endl;
            n.setAddress("/wekinator/control/startDtwRecording");
           
            senderActivation.sendMessage(n);
            
//          cout << "message out = " << m.getAddress() << endl;
            
            if (switchedOn == true){
                //if (RMS < 2){
                    //cout << "RMS Off = " << RMS << endl;
                    switchedOn = false;
                    //cout << "2. OSC Out switch = " << std::boolalpha << switchedOn << endl;
                    o.setAddress("/wekinator/control/stopDtwRecording");
                    senderDeactivation.sendMessage(o);
                    switchedOn = false;
                }
            //}
        }
    }
    
    // FAILED ATTEMPTS???
    
    // After many different attempted configurations of this if statement with a boolean for switching on and off I have ended up just sending both messages at the same time andf am applying a delay in the max patch (see max patch).
    
    // I tried nesting the if statements, I tried the else if statement. I just couldnt get it happening. the gate and switch approach in Max also proved unfruitful. In the end Occams razor and this hacky method proved to do the trick.
        
//        if (switchedOn == true){
//            if (RMS < 2){
//                cout << "RMS Off = " << RMS << endl;
//                cout << "2. OSC Out switch = " << std::boolalpha << switchedOn << endl;
//                o.setAddress("/wekinator/control/stopDtwRecording 1");
//                senderDeactivation.sendMessage(o);
//                switchedOn = false;
//            }
        
//        if (RMS < 2) {
//            if (switchedOn == true){
//            cout << "RMS Off = " << RMS << endl;
//            cout << "2. OSC Out switch = " << std::boolalpha << switchedOn << endl;
//            o.setAddress("/wekinator/control/stopDtwRecording 1");
//            senderDeactivation.sendMessage(o);
//            switchedOn = false;
//
//        }
//    }
    


    
    
        // ----- receive OSC -----
        receiver.getNextMessage(&m);
        
        /* at the moment messages being received are hardcoded. The web idea is supposed to allow for more dynamic selection of class messages. A for loop iterating through a list of messages sent by wekinator. This most likely means better knowledge of the osc class within OF to que messages and work with lists of messages */
        // get class 1 for activation
        if(m.getAddress() == "/output_1"){
            messages = m.getAddress();
            // get the first argument (we're only sending one) as a string
            cout << "message = " << messages << endl;
        
        // get class 2 for activation
        } else if (m.getAddress() == "/output_2"){
            messages = m.getAddress();
            // get the first argument (we're only sending one) as a string
            cout << "message = " << messages << endl;
    }

    
    // This is how to send message to weki to sart recording.
    // http://www.wekinator.org/detailed-instructions/#Customizing_DTW8217s_behavior
    // /wekinator/control/startDtwRecording
    // this will be used for training the weki models with guitar, cajon and dance etc
    // might be good for setting up the project's models to be trained on the fly

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    float horizWidth = 500.;
    float horizOffset = 100;
    //float fftTop = 250;
    float mfccTop = 350;
    //float chromagramTop = 450;
    
    ofSetColor(255, 0, 0,255);
    
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
    for(int i = 0; i < 13; i++) {
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
        
         float ampOut = 0.5;
        
         spitOut = mySine1.sinewave(500);
        
         lAudioOut[i * nChannels] = spitOut * ampOut;
         rAudioOut[i * nChannels + 1] = spitOut * ampOut;
        
        std::cout << "spitOut = " << spitOut << endl;
        
    }
}

//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels) {
    
    // float ampOut = 0.5;
    
    // spitOut = mySine1.sinewave(500);
    for (int i = 0; i < bufferSize; i++){

//        output[i*nChannels    ] = spitOut * ampOut; /* You may end up with lots of outputs. add them here */
//        output[i*nChannels + 1] = spitOut * ampOut;
        
    }
    
}

//----------------------------------------------------------------------------

void ofApp::audioReceived     (float * input, int bufferSize, int nChannels){
			
    /* You can just grab this input and stick it in a double, then use it above to create output*/
    
    float sum = 0;
    for (int i = 0; i < bufferSize; i++){
        
        /* you can also grab the data out of the arrays*/
        
        lAudioIn[i] = input[i*2];
        rAudioIn[i] = input[i*2+1];
        
        sum += input[i*2] * input[i*2];
        spitOut = sum;
        
        
        
    }
    RMS = sqrt(sum);
    // cout << " Raw RMS = " << RMS << endl;
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
//
//    ofxOscMessage m, n, o;
//
//    if (key == 'a'){
//        n.setAddress("/wekinator/control/startDtwRecording 1");
//        m.setAddress("mfccs");
//        // o.setAddress("/wekinator/control/stopDtwRecording 1");
//        for (int i = 0; i < 13; i++) {
//            m.addFloatArg(mfccs[i]);
//        }
//
//        senderActivation.sendMessage(n);
//        sender.sendMessage(m);
//        cout << "message out = " << m.getAddress() << endl;
//    } else {
//
//
//
//        }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
//    ofxOscMessage o;
//    o.setAddress("/wekinator/control/stopDtwRecording 1");
//    senderDeactivation.sendMessage(o);
    
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

