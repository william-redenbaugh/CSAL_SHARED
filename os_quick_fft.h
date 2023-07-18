#ifndef _OS_QUICK_FFT
#define _OS_QUICK_FFT

/**
    Code  to perform High speed (5-7 times faster) and low accuracy FFT on arduino,
    This  code compromises accuracy for speed,
    setup:

    1. in[]     : Data array,
    2. N        : Number of sample (recommended sample size 2,4,8,16,32,64,128...)
    3.  Frequency: sampling frequency required as input (Hz)

    @param int int[] data array in place calculation
    @param int N 2^N samples
    @param float frequency: sampling frequency of data collection

    It will by default return  frequency with max aplitude,

    If sample size is not in power of 2 it will  be clipped to lower side of number.
    i.e, for 150 number of samples, code will  consider first 128 sample, remaining sample  will be omitted.
    For Arduino nano,  FFT of more than 256 sample not possible due to mamory limitation
    Code by ABHILASH
    Contact:  abhilashpatel121@gmail.com
Documentation & deatails: https://www.instructables.com/member/abhilash_patel/instructables/
*/
float Q_FFT(int in[], int N, float Frequency);

#endif