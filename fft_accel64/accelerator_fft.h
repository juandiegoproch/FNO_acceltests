#ifndef ACCELERATOR_FFT
#define ACCELERATOR_FFT

#include <stdint.h>

typedef union {
    uint64_t val; // The single 32-bit field for hardware registers
    struct {
        uint32_t i; // Maps to bits [15:0] (Imaginary)
        uint32_t r; // Maps to bits [31:16] (Real)
    };
} cpx_32bq16_accel;

/*
Naming convention for FFT functions is <whatitdoes><size>-<byteformat>_accel
*/

void rfft32_32bq16_accel(cpx_32bq16_accel* buffer);
void ifft32_32bq16_accel(cpx_32bq16_accel* buffer);

void fft32x32_32bq16_accel(cpx_32bq16_accel* buffer);
void ifft32x32_32bq16_accel(cpx_32bq16_accel* buffer);

// 64-point support built on the fixed 32-point hardware (Cooley-Tukey radix-2).
void fft64_32bq16_accel(cpx_32bq16_accel* buffer);     // 1D, 64 puntos
void fft64x64_32bq16_accel(cpx_32bq16_accel* buffer);  // 2D
void ifft64x64_32bq16_accel(cpx_32bq16_accel* buffer); // 2D inversa

#endif
