#include "accelerator_fft.h"

#define FFT_WRITE_LANE   (uintptr_t)0x2400
#define FFT_RD_LANE_BASE (uintptr_t)0x2408
#define MATRIX_DIM       32
#define TOTAL_POINTS     (MATRIX_DIM * MATRIX_DIM)


void rfft32_32bq16_accel(cpx_32bq16_accel* buffer) {
    volatile uint64_t* write_ptr = (volatile uint64_t*)FFT_WRITE_LANE;
    for (int n = 0; n < MATRIX_DIM; n++) {
        *write_ptr = buffer[n].val;
    }
    for (int k = 0; k < MATRIX_DIM; k++) {
        volatile uint64_t* read_ptr = (volatile uint64_t*)(FFT_RD_LANE_BASE + (k * 8));
        buffer[k].val = *read_ptr;
    }
}

static void transpose_32x32(cpx_32bq16_accel* matrix) {
    cpx_32bq16_accel temp;
    for (int i = 0; i < MATRIX_DIM; i++) {
        for (int j = i + 1; j < MATRIX_DIM; j++) {
            int idx1 = i * MATRIX_DIM + j;
            int idx2 = j * MATRIX_DIM + i;
            
            temp         = matrix[idx1];
            matrix[idx1] = matrix[idx2];
            matrix[idx2] = temp;
        }
    }
}
void fft32x32_32bq16_accel(cpx_32bq16_accel* buffer) {
    // Row Pass
    for (int r = 0; r < MATRIX_DIM; r++) {
        rfft32_32bq16_accel(&buffer[r * MATRIX_DIM]);
    }

    transpose_32x32(buffer);
    for (int c = 0; c < MATRIX_DIM; c++) {
        rfft32_32bq16_accel(&buffer[c * MATRIX_DIM]);
    }
    transpose_32x32(buffer);
}

void ifft32x32_32bq16_accel(cpx_32bq16_accel* buffer) {
    for (int i = 0; i < TOTAL_POINTS; i++) {
        uint32_t temp = buffer[i].r;
        buffer[i].r = buffer[i].i;
        buffer[i].i = temp;
    }
    fft32x32_32bq16_accel(buffer);

    for (int i = 0; i < TOTAL_POINTS; i++) {
        int32_t signed_r = (int32_t)buffer[i].i;
        int32_t signed_i = (int32_t)buffer[i].r;

        buffer[i].r = (uint32_t)(signed_r >> 10);
        buffer[i].i = (uint32_t)(signed_i >> 10);
    }
}
