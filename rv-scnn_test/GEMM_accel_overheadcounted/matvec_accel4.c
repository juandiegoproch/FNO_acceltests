#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "chipyard_compatibility.h"
#include "../include/custom.h"

#define MAX_M 32
#define MAX_N 32
#define MAX_K 32

static int8_t  A_tile[MAX_K * 4] __attribute__((aligned(128)));
static int8_t  B_tile[MAX_K * 4] __attribute__((aligned(128)));
static int32_t C_tile[16]        __attribute__((aligned(128)));
static int8_t  A[MAX_M * MAX_K] __attribute__((aligned(128)));
static int8_t  B[MAX_K * MAX_N] __attribute__((aligned(128)));
static int32_t C_hw[MAX_M * MAX_N] __attribute__((aligned(128)));

/* Accumulated accelerator-only cycles and instructions across all tiles */
static uint64_t accel_cycles_accum;
static uint64_t accel_instret_accum;

void gemm(int M, int N, int K, int8_t *A, int8_t *B, int32_t *C) {
    for (int ti = 0; ti < M/4; ti++) {
        for (int tj = 0; tj < N/4; tj++) {

            /* --- gather A tile (column-major strip) --- */
            for (int k = 0; k < K; k++)
                for (int i = 0; i < 4; i++)
                    A_tile[k*4 + i] = A[k*M + ti*4 + i];

            /* --- gather B tile --- */
            for (int k = 0; k < K; k++)
                for (int j = 0; j < 4; j++)
                    B_tile[k*4 + j] = B[k*N + tj*4 + j];

            for (int i = 0; i < 16; i++) C_tile[i] = 0;

            /* ---- accelerator fence: start ---- */
            uint64_t ac0 = read_cycles();
            uint64_t ai0 = read_instret();

            L_SCNN(K, 4, 4, 0);
            SCNN4x4(A_tile, B_tile);
            SCNN_WB_INT((int *)C_tile);

            uint64_t ac1 = read_cycles();
            uint64_t ai1 = read_instret();
            /* ---- accelerator fence: end ---- */

            accel_cycles_accum  += ac1 - ac0;
            accel_instret_accum += ai1 - ai0;

            /* --- scatter C tile --- */
            for (int r = 0; r < 4; r++)
                for (int c = 0; c < 4; c++)
                    C[(ti*4 + r)*N + (tj*4 + c)] = C_tile[r*4 + c];
        }
    }
}

void gemm_unaccel(int M, int N, int K, int8_t *A, int8_t *B, int32_t *C) {
    for (int i = 0; i < M*N; i++) C[i] = 0;
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            for (int k = 0; k < K; k++)
                C[i*N + j] += (int32_t)A[k*M + i] * (int32_t)B[k*N + j];
}

void instrument(const char *name, int M, int N, int K) {
    for (int i = 0; i < M*K; i++) A[i] = (i % 11) - 5;
    for (int i = 0; i < K*N; i++) B[i] = (i % 9)  - 4;
    for (int i = 0; i < M*N; i++) C_hw[i] = 0;

    /* Reset per-run accumulators */
    accel_cycles_accum  = 0;
    accel_instret_accum = 0;

    uint64_t c0 = read_cycles();
    uint64_t i0 = read_instret();

    gemm(M, N, K, A, B, C_hw);

    uint64_t c1 = read_cycles();
    uint64_t i1 = read_instret();

    uint64_t total_cycles  = c1 - c0;
    uint64_t total_instret = i1 - i0;

    /* Clamp: counter noise can make accel > total on tiny cases */
    uint64_t scatter_cycles  = (accel_cycles_accum  <= total_cycles)
                               ? total_cycles  - accel_cycles_accum  : 0;
    uint64_t scatter_instret = (accel_instret_accum <= total_instret)
                               ? total_instret - accel_instret_accum : 0;

    int num_tiles = (M/4) * (N/4);

    printf("%s:\n", name);
    printf("  total          : cycles=%llu  instret=%llu\n",
           (unsigned long long)total_cycles,
           (unsigned long long)total_instret);
    printf("  accel only     : cycles=%llu  instret=%llu\n",
           (unsigned long long)accel_cycles_accum,
           (unsigned long long)accel_instret_accum);
    printf("  gather/scatter : cycles=%llu  instret=%llu\n",
           (unsigned long long)scatter_cycles,
           (unsigned long long)scatter_instret);
    printf("  tiles          : %d  (accel cycles/tile=%llu)\n",
           num_tiles,
           (unsigned long long)(num_tiles ? accel_cycles_accum / num_tiles : 0));
}

int main(void) {
    instrument("4x4 * 4x4",     4,  4,  4);
    instrument("8x8 * 8x8",     8,  8,  8);
    instrument("16x16 * 16x16", 16, 16, 16);
    instrument("32x32 * 32x32", 32, 32, 32);
    return 0;
}
