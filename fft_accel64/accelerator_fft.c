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

/* ============================================================
 * 64-point support via Cooley-Tukey radix-2.
 * The hardware stays fixed at 32 points: a 64-point FFT is built
 * from TWO hardware 32-point FFTs (even/odd halves) plus one
 * software combine stage with W_64^k twiddles.
 * ============================================================ */

#define MATRIX_DIM_64   64
#define TOTAL_POINTS_64 (MATRIX_DIM_64 * MATRIX_DIM_64)

/* Twiddle factors W_64^k = exp(-j*2*pi*k/64), k=0..31, in Q15.16.
 * .r/.i are stored as uint32_t but represent SIGNED Q15.16 values,
 * so always read them through an int32_t cast in the arithmetic.
 * Designated initializers are used so the order of the union's
 * struct fields (i first, r second) is irrelevant. */
static const cpx_32bq16_accel tw64[32] = {
  {.r=65536,.i=0},{.r=65220,.i=-6424},{.r=64277,.i=-12785},{.r=62714,.i=-19024},
  {.r=60547,.i=-25080},{.r=57798,.i=-30893},{.r=54491,.i=-36410},{.r=50660,.i=-41576},
  {.r=46341,.i=-46341},{.r=41576,.i=-50660},{.r=36410,.i=-54491},{.r=30893,.i=-57798},
  {.r=25080,.i=-60547},{.r=19024,.i=-62714},{.r=12785,.i=-64277},{.r=6424,.i=-65220},
  {.r=0,.i=-65536},{.r=-6424,.i=-65220},{.r=-12785,.i=-64277},{.r=-19024,.i=-62714},
  {.r=-25080,.i=-60547},{.r=-30893,.i=-57798},{.r=-36410,.i=-54491},{.r=-41576,.i=-50660},
  {.r=-46341,.i=-46341},{.r=-50660,.i=-41576},{.r=-54491,.i=-36410},{.r=-57798,.i=-30893},
  {.r=-60547,.i=-25080},{.r=-62714,.i=-19024},{.r=-64277,.i=-12785},{.r=-65220,.i=-6424}
};

/**
 * @brief 1D Forward FFT (64-point) on the 32-point hardware.
 * One radix-2 DIT stage: deinterleave into even/odd, run two HW
 * 32-point FFTs, then combine with W_64^k twiddles.
 */
void fft64_32bq16_accel(cpx_32bq16_accel* buffer) {
    cpx_32bq16_accel even[MATRIX_DIM];
    cpx_32bq16_accel odd[MATRIX_DIM];

    // Deinterleave par/impar
    for (int m = 0; m < MATRIX_DIM; m++) {
        even[m] = buffer[2 * m];
        odd[m]  = buffer[2 * m + 1];
    }

    // The TWO hardware 32-point FFTs
    rfft32_32bq16_accel(even);
    rfft32_32bq16_accel(odd);

    // Combine stage. No intermediate scaling (same criterion as the 32x32 accel).
    for (int k = 0; k < MATRIX_DIM; k++) {
        cpx_32bq16_accel tw = tw64[k];
        cpx_32bq16_accel E  = even[k];
        cpx_32bq16_accel O  = odd[k];

        // Complex multiply O * tw in int64, then >>16 back to Q15.16
        int64_t twr = ((int64_t)(int32_t)O.r * (int32_t)tw.r
                     - (int64_t)(int32_t)O.i * (int32_t)tw.i) >> 16;
        int64_t twi = ((int64_t)(int32_t)O.r * (int32_t)tw.i
                     + (int64_t)(int32_t)O.i * (int32_t)tw.r) >> 16;

        buffer[k].r              = (uint32_t)((int32_t)E.r + (int32_t)twr);
        buffer[k].i              = (uint32_t)((int32_t)E.i + (int32_t)twi);
        buffer[k + MATRIX_DIM].r = (uint32_t)((int32_t)E.r - (int32_t)twr);
        buffer[k + MATRIX_DIM].i = (uint32_t)((int32_t)E.i - (int32_t)twi);
    }
}

/**
 * @brief In-place matrix transposition for 64x64 grid.
 */
static void transpose_64x64(cpx_32bq16_accel* matrix) {
    cpx_32bq16_accel temp;
    for (int i = 0; i < MATRIX_DIM_64; i++) {
        for (int j = i + 1; j < MATRIX_DIM_64; j++) {
            int idx1 = i * MATRIX_DIM_64 + j;
            int idx2 = j * MATRIX_DIM_64 + i;

            temp         = matrix[idx1];
            matrix[idx1] = matrix[idx2];
            matrix[idx2] = temp;
        }
    }
}

/**
 * @brief 2D Forward FFT (64x64), row-column decomposition.
 * High-precision pass with no intermediate bit-shifting.
 */
void fft64x64_32bq16_accel(cpx_32bq16_accel* buffer) {
    // Row Pass
    for (int r = 0; r < MATRIX_DIM_64; r++) {
        fft64_32bq16_accel(&buffer[r * MATRIX_DIM_64]);
    }

    transpose_64x64(buffer);

    // Column Pass (Now row-aligned)
    for (int c = 0; c < MATRIX_DIM_64; c++) {
        fft64_32bq16_accel(&buffer[c * MATRIX_DIM_64]);
    }

    // Restore original orientation
    transpose_64x64(buffer);
}

/**
 * @brief 2D Inverse FFT (64x64)
 * Correctly scales by 1/(N*M) = 1/4096 at the final stage only.
 */
void ifft64x64_32bq16_accel(cpx_32bq16_accel* buffer) {
    // 1. Pre-swap Real and Imaginary components
    for (int i = 0; i < TOTAL_POINTS_64; i++) {
        uint32_t temp = buffer[i].r;
        buffer[i].r = buffer[i].i;
        buffer[i].i = temp;
    }

    // 2. Run Forward FFT core
    fft64x64_32bq16_accel(buffer);

    // 3. Post-swap and apply the global 1/4096 scale (12-bit shift)
    for (int i = 0; i < TOTAL_POINTS_64; i++) {
        // Swap components back to original positions
        int32_t signed_r = (int32_t)buffer[i].i;
        int32_t signed_i = (int32_t)buffer[i].r;

        // Final normalization shift (64 * 64 = 4096)
        // Using arithmetic shift to preserve sign bits
        buffer[i].r = (uint32_t)(signed_r >> 12);
        buffer[i].i = (uint32_t)(signed_i >> 12);
    }
}
