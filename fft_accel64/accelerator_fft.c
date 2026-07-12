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

void fft64_32bq16_accel(cpx_32bq16_accel* buffer) {
    cpx_32bq16_accel even[MATRIX_DIM];
    cpx_32bq16_accel odd[MATRIX_DIM];

   // separar en 2 por par/inpar
    for (int m = 0; m < MATRIX_DIM; m++) {
        even[m] = buffer[2 * m];
        odd[m]  = buffer[2 * m + 1];
    }

    // hojas
    rfft32_32bq16_accel(even);
    rfft32_32bq16_accel(odd);

    // combinar con twiddles
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

// 64x64 usando la primitiva FFT64
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

// 64x64 usando la primitiva FFT64
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
