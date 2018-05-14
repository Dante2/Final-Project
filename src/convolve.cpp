//
//  convolve.cpp
//  oscSenderExample
//
//  Created by Jools on 14/05/2018.
//

#include "convolve.hpp"

void convolve::setUp(int rateIn, int averages, int size){
    
    sized = size;
    rated = rateIn;
    averaged = averages;
    mfft1.setup(sized, 512, 256);
    mfft2.setup(sized, 512, 256);
    ifft.setup(sized, 512, 256);
    oct.setup(rated, sized/2, averages);
    
}

double convolve::convolving(bool play, double signal, double signal2){
    
    convolve = play;
    if (convolve){
    
        waveIn = signal;
        waveIn2 = signal2;

        if (mfft1.process(waveIn)) {
            int bins   = fftSize / 2.0;
            mfft1.magsToDB();
            oct.calculate(mfft1.magnitudesDB);
            float sum = 0;
            float maxFreq = 0;
            int maxBin = 0;
            for (int i = 0; i < fftSize/2; i++) {
                sum += mfft1.magnitudes[i];
                if (mfft1.magnitudes[i] > maxFreq) {
                    maxFreq = mfft1.magnitudes[i];
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
        convolveOut = ifft.process(mfft1.magnitudes, mfft2.phases);
    }
}
