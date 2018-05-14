//
//  convolve.hpp
//  oscSenderExample
//
//  Created by Jools on 14/05/2018.
//

#ifndef convolve_hpp
#define convolve_hpp

#include <stdio.h>
#include <iostream>
#include "ofxMaxim.h"

class convolve  {
    
    ofxMaxiIFFT ifft;
    ofxMaxiFFT mfft, mfft2;
    ofxMaxiFFTOctaveAnalyzer oct;
    int bins, dataSize, averaged, sized, rated, fftSize;
    double waveIn, waveIn2, fftconv1, fftconv2;
    bool convolve;
 
public:
    
    void setUp(int rateIn, int averages, int size);
    double convolving(bool play, double signal, double signal2);
    double convolveOut;
    
};

#endif /* convolve_hpp */
