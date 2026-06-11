
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "chipyard_compatibility.h"
#include "../include/custom.h"


#ifndef SZCH
#define SZCH 32
#endif
#define L      (SZCH * SZCH)     
#define CIN0   24
#define CMID   12
#define COUT   24

#define HEAP_SIZE (128 * 1024)

// --- 1. THE HARDWARE PRIMITIVE (Black Box) ---
typedef void (*gemm4x4_func)(int8_t *a_tile, int8_t *b_tile, int32_t *res_tile);

void gemm4x4_hw(int8_t *a_tile, int8_t *b_tile, int32_t *res_tile) {
    L_SCNN(4, 4, 4, 0); 
    L_MODE(0, 1, 0, 0); 
    SCNN4x4(a_tile, b_tile);
    SCNN_WB_INT((int *)res_tile);
}

// --- 2. THE ACCELERATED ORCHESTRATOR ---
// Handles arbitrary M, N, K with virtual zero-padding for hardware safety
void GEMM_Accelerated(int M, int N, int K, elem_t *A, elem_t *B, elem_t *C) {
    static int8_t  a_tile[16]   __attribute__((aligned(128)));
    static int8_t  b_tile[16]   __attribute__((aligned(128)));
    static int32_t res_tile[16] __attribute__((aligned(128)));

    for (int i = 0; i < M; i += 4) {
        for (int j = 0; j < N; j += 4) {
            int32_t acc[16] = {0};

            for (int k = 0; k < K; k += 4) {
                // Packing with bounds checking (Virtual Padding)
                for (int tk = 0; tk < 4; tk++) {
                    for (int ti = 0; ti < 4; ti++) {
                        if ((i + ti) < M && (k + tk) < K)
                            a_tile[tk * 4 + ti] = A[(i + ti) * K + (k + tk)];
                        else
                            a_tile[tk * 4 + ti] = 0;
                    }
                    for (int tj = 0; tj < 4; tj++) {
                        if ((k + tk) < K && (j + tj) < N)
                            b_tile[tk * 4 + tj] = B[(k + tk) * N + (j + tj)];
                        else
                            b_tile[tk * 4 + tj] = 0;
                    }
                }

                gemm4x4_hw(a_tile, b_tile, res_tile);

                for (int n = 0; n < 16; n++) acc[n] += res_tile[n];
            }

            // Stitching back with saturation and bounds checking
            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {
                    if ((i + r) < M && (j + c) < N) {
                        int32_t final_acc = acc[r * 4 + c];
                        if (final_acc > 127) final_acc = 127;
                        if (final_acc < -128) final_acc = -128;
                        C[(i + r) * N + (j + c)] = (elem_t)final_acc;
                    }
                }
            }
        }
    }
}

int main(void) {
    printf("=== Accelerated: matvec ChannelMLP (RV-SCNN) ===\n");
    printf("SZCH=%d L=%d CIN0=%d CMID=%d COUT=%d\n\n", SZCH, L, CIN0, CMID, COUT);

    static uint8_t heap[HEAP_SIZE];
    elem_t *A0 = (elem_t *)(&heap[0]);
    elem_t *W1 = (elem_t *)(A0 + (size_t)L * CIN0);
    elem_t *C1 = (elem_t *)(W1 + (size_t)CIN0 * CMID);
    elem_t *W2 = (elem_t *)(C1 + (size_t)L * CMID);
    elem_t *C2 = (elem_t *)(W2 + (size_t)CMID * COUT);

    for (int i = 0; i < L; ++i) {
        int ones = i % 8;
        for (int k = 0; k < CIN0; ++k)
            A0[(size_t)i * CIN0 + k] = (elem_t)(k < ones ? 1 : 0);
    }
    for (int i = 0; i < CIN0 * CMID; i++) W1[i] = (elem_t)1;
    for (int i = 0; i < CMID * COUT; i++) W2[i] = (elem_t)1;

    uint64_t c_start = read_cycles();
    uint64_t i_start = read_instret();

    // Layer 1: L x CIN0 * CIN0 x CMID -> L x CMID
    GEMM_Accelerated(L, CMID, CIN0, A0, W1, C1);
    
    // Layer 2: L x CMID * CMID x COUT -> L x COUT
    GEMM_Accelerated(L, COUT, CMID, C1, W2, C2);

    uint64_t i_end = read_instret();
    uint64_t c_end = read_cycles();

    uint64_t cycles  = c_end - c_start;
    uint64_t instret = i_end - i_start;
    printf("Accelerated matvec: %llu cycles, %llu instructions\n",
           (unsigned long long)cycles, (unsigned long long)instret);


    int ok = 1;
    for (int i = 0; i < L && ok; ++i) {
        elem_t expected = (elem_t)(CMID * (i % 8));
        for (int j = 0; j < COUT; ++j)
            if (C2[(size_t)i * COUT + j] != expected) { ok = 0; break; }
    }
    printf("Verificacion HW: %s\n", ok ? "PASS" : "FAIL");

    return ok ? 0 : 1;
}