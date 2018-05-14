//
//  convolve.cpp
//  oscSenderExample
//
//  Created by Jools on 14/05/2018.
//

#include "convolve.hpp"

void convolve::setUp(int rateIn, int averages){
    
    int rated = rateIn;
    int averaged = averages;
    fftSize = 1024;
    mfft.setup(fftSize, 512, 256);
    mfft2.setup(fftSize, 512, 256);
    ifft.setup(fftSize, 512, 256);
    oct.setup(rated, fftSize/2, averages);
    
}

double convolve::convolving(bool play, double signal, double signal2){
    
    convolve = play;
    if (convolve){
    
        waveIn = signal;
        waveIn2 = signal2;

        if (mfft.process(waveIn)) {
            int bins   = fftSize / 2.0;
            mfft.magsToDB();
            oct.calculate(mfft.magnitudesDB);
            float sum = 0;
            float maxFreq = 0;
            int maxBin = 0;
            for (int i = 0; i < fftSize/2; i++) {
                sum += mfft.magnitudes[i];
                if (mfft.magnitudes[i] > maxFreq) {
                    maxFreq = mfft.magnitudes[i];
                    maxBin = i;
                }
            }
        }
        
        if (mfft2.process(waveIn2)) {
            int bins   = fftSize / 2.0;
            mfft2.magsToDB();
            oct.calculate(mfft2.magnitudesDB);
            float sum = 0;
            float maxFreq = 0;
            int maxBin = 0;
            for (int i = 0; i < fftSize/2; i++) {
                sum += mfft2.magnitudes[i];
                if (mfft2.magnitudes[i] > maxFreq) {
                    maxFreq = mfft2.magnitudes[i];
                    maxBin = i;
                }
            }
        }
        convolveOut = ifft.process(mfft.magnitudes, mfft2.phases);
        // cout << "convOut = " << convolveOut << endl;
    }
}

//double convolve::convolveOut( double mags, double phase){
//
//    double magnitudes = mags;
//    double phases = phase;
//    
//    ifft.process(mfft.magnitudes, mfft.phases);

