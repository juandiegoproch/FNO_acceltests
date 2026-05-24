#include "accelerator_fft.h"

// Note: Address must be 64-bit aligned for uint64_t access
#define FFT_WRITE_LANE   (uintptr_t)0x2400
#define FFT_RD_LANE_BASE (uintptr_t)0x2408
#define MATRIX_DIM       32
#define TOTAL_POINTS     (MATRIX_DIM * MATRIX_DIM)

/**
 * @brief 1D Forward FFT Driver (32-point, 64-bit complex)
 * Sends 32-bit Real and 32-bit Imaginary in a single 64-bit bus transaction.
 */
void rfft32_32bq16_accel(cpx_32bq16_accel* buffer) {
    volatile uint64_t* write_ptr = (volatile uint64_t*)FFT_WRITE_LANE;
    
    // 1. Write the 32 complex data points to the accelerator
    for (int n = 0; n < MATRIX_DIM; n++) {
        *write_ptr = buffer[n].val;
    }

    // 2. Read the results back. 
    // No intermediate scaling is required thanks to the 32-bit component width.
    for (int k = 0; k < MATRIX_DIM; k++) {
        volatile uint64_t* read_ptr = (volatile uint64_t*)(FFT_RD_LANE_BASE + (k * 8));
        buffer[k].val = *read_ptr;
    }
}

/**
 * @brief In-place matrix transposition for 32x32 grid.
 */
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

/**
 * @brief 2D Forward FFT (32x32)
 * High-precision pass with no intermediate bit-shifting.
 */
void fft32x32_32bq16_accel(cpx_32bq16_accel* buffer) {
    // Row Pass
    for (int r = 0; r < MATRIX_DIM; r++) {
        rfft32_32bq16_accel(&buffer[r * MATRIX_DIM]);
    }

    transpose_32x32(buffer);

    // Column Pass (Now row-aligned)
    for (int c = 0; c < MATRIX_DIM; c++) {
        rfft32_32bq16_accel(&buffer[c * MATRIX_DIM]);
    }

    // Restore original orientation
    transpose_32x32(buffer);
}

/**
 * @brief 2D Inverse FFT (32x32)
 * Correctly scales by 1/(N*M) = 1/1024 at the final stage only.
 */
void ifft32x32_32bq16_accel(cpx_32bq16_accel* buffer) {
    // 1. Pre-swap Real and Imaginary components
    for (int i = 0; i < TOTAL_POINTS; i++) {
        uint32_t temp = buffer[i].r;
        buffer[i].r = buffer[i].i;
        buffer[i].i = temp;
    }

    // 2. Run Forward FFT core
    fft32x32_32bq16_accel(buffer);

    // 3. Post-swap and apply the global 1/1024 scale (10-bit shift)
    for (int i = 0; i < TOTAL_POINTS; i++) {
        // Swap components back to original positions
        int32_t signed_r = (int32_t)buffer[i].i;
        int32_t signed_i = (int32_t)buffer[i].r;

        // Final normalization shift (32 * 32 = 1024)
        // Using arithmetic shift to preserve sign bits
        buffer[i].r = (uint32_t)(signed_r >> 10);
        buffer[i].i = (uint32_t)(signed_i >> 10);
    }
}