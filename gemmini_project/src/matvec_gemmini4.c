#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "include/gemmini_testutils.h"

static inline uint64_t read_instret(void) {
    uint64_t x;
    asm volatile ("rdinstret %0" : "=r" (x));
    return x;
}

int main(void) {
    printf("=== Fast 4x4 single-shot GEMM (Gemmini) ===\n");

    // Same inputs as RV-SCNN test: A=identity, B=1..16
    static elem_t A[16] __attribute__((aligned(64)));
    static elem_t B[16] __attribute__((aligned(64)));
    static elem_t C[16] __attribute__((aligned(64)));

    // A: identity (row-major, no row-flip needed — Gemmini handles layout)
    A[ 0]=1; A[ 1]=0; A[ 2]=0; A[ 3]=0;
    A[ 4]=0; A[ 5]=1; A[ 6]=0; A[ 7]=0;
    A[ 8]=0; A[ 9]=0; A[10]=1; A[11]=0;
    A[12]=0; A[13]=0; A[14]=0; A[15]=1;

    // B: 1..16
    B[ 0]=1;  B[ 1]=2;  B[ 2]=3;  B[ 3]=4;
    B[ 4]=5;  B[ 5]=6;  B[ 6]=7;  B[ 7]=8;
    B[ 8]=9;  B[ 9]=10; B[10]=11; B[11]=12;
    B[12]=13; B[13]=14; B[14]=15; B[15]=16;

    for (int i = 0; i < 16; i++) C[i] = 0;

    gemmini_flush(0);

    uint64_t c_start = read_cycles();
    uint64_t i_start = read_instret();

    // Single 4x4x4 GEMM: C = A * B
    tiled_matmul_auto(
        4, 4, 4,            // dim_I, dim_J, dim_K
        A, B, NULL, C,      // A, B, D(bias)=NULL, C
        4, 4, 4, 4,         // stride_A, stride_B, stride_D, stride_C
        MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
        NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, true,
        false, false,       // transpose_A, transpose_B
        false, false,       // full_C, low_D
        0,
        WS);

    gemmini_fence();

    uint64_t i_end = read_instret();
    uint64_t c_end = read_cycles();

    printf("Cycles: %llu  Instructions: %llu\n",
           (unsigned long long)(c_end - c_start),
           (unsigned long long)(i_end - i_start));

    // A=I so expected C=B, no row-flip needed
    int ok = 1;
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++) {
            int got      = (int)C[r*4+c];
            int expected = (int)B[r*4+c];
            printf("  C[%d][%d] = %4d  (expected %4d)%s\n",
                   r, c, got, expected,
                   got != expected ? "  <-- FAIL" : "");
            if (got != expected) ok = 0;
        }

    printf("Result: %s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
