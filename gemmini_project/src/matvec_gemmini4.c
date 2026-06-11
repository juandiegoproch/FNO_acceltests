#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "include/gemmini_testutils.h"

static inline uint64_t read_instret(void) {
    uint64_t x;
    asm volatile ("rdinstret %0" : "=r" (x));
    return x;
}

#define MAX_M 32
#define MAX_N 32
#define MAX_K 32

static elem_t  A[MAX_M * MAX_K] __attribute__((aligned(128)));
static elem_t  B[MAX_K * MAX_N] __attribute__((aligned(128)));
static acc_t   C[MAX_M * MAX_N] __attribute__((aligned(128)));

void instrument_accel(const char *name, int M, int N, int K) {
    for (int i = 0; i < M*K; i++) A[i] = (i % 11) - 5;
    for (int i = 0; i < K*N; i++) B[i] = (i % 9)  - 4;
    for (int i = 0; i < M*N; i++) C[i] = 0;

    // warmup
    gemmini_flush(0);
    tiled_matmul_auto(
        M, N, K,
        A, B, NULL, C,
        K, N, N, N,
        MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
        NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, true,
        false, false,
        false, false,
        0,
        WS);
    gemmini_fence();

    // measured run
    gemmini_flush(0);
    uint64_t c0 = read_cycles();
    uint64_t i0 = read_instret();

    tiled_matmul_auto(
        M, N, K,
        A, B, NULL, C,
        K, N, N, N,
        MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
        NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, true,
        false, false,
        false, false,
        0,
        WS);
    gemmini_fence();

    uint64_t c1 = read_cycles();
    uint64_t i1 = read_instret();

    printf("%s: cycles=%llu instret=%llu\n",
        name,
        (unsigned long long)(c1 - c0),
        (unsigned long long)(i1 - i0));
}

int main(void) {
    instrument_accel("4x4 * 4x4",    4,  4,  4);
    instrument_accel("8x8 * 8x8",    8,  8,  8);
    instrument_accel("16x16 * 16x16",16, 16, 16);
    instrument_accel("32x32 * 32x32",32, 32, 32);
    return 0;
}