#ifndef ACCELERATOR_FFT
#define ACCELERATOR_FFT

#include <stdint.h>

typedef union {
    uint64_t val; 
    struct {
        uint32_t i; // Maps to bits [15:0] (Imaginary)
        uint32_t r; // Maps to bits [31:16] (Real)
    };
} cpx_32bq16_accel;

void rfft32_32bq16_accel(cpx_32bq16_accel* buffer);
void ifft32_32bq16_accel(cpx_32bq16_accel* buffer);

void fft32x32_32bq16_accel(cpx_32bq16_accel* buffer);
void ifft32x32_32bq16_accel(cpx_32bq16_accel* buffer);

#endif
