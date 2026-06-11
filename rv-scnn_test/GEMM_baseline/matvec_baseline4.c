#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_M 32
#define MAX_N 32
#define MAX_K 32

static int8_t  A_tile[MAX_K * 4] __attribute__((aligned(128)));
static int8_t  B_tile[MAX_K * 4] __attribute__((aligned(128)));
static int32_t C_tile[16]        __attribute__((aligned(128)));

static int8_t  A[MAX_M * MAX_K] __attribute__((aligned(128)));
static int8_t  B[MAX_K * MAX_N] __attribute__((aligned(128)));
static int32_t C_hw[MAX_M * MAX_N] __attribute__((aligned(128)));

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

    uint64_t c0 = read_cycles();
    uint64_t i0 = read_instret();

    gemm(M, N, K, A, B, C_hw);

    uint64_t c1 = read_cycles();
    uint64_t i1 = read_instret();

    printf("%s: cycles=%llu instret=%llu\n",
        name,
        (unsigned long long)(c1 - c0),
        (unsigned long long)(i1 - i0));
}

int main(void) {
    instrument("4x4 * 4x4",    4,  4,  4);
    instrument("8x8 * 8x8",    8,  8,  8);
    instrument("16x16 * 16x16",16, 16, 16);
    instrument("32x32 * 32x32",32, 32, 32);
    return 0;
}