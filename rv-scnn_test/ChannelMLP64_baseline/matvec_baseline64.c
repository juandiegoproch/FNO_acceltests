#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "chipyard_compatibility.h"
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
//#include "include/gemmini_testutils.h"   // elem_t (int8), acc_t (int32), read_cycles()

#ifndef SZCH
#define SZCH 64
#endif
#define L      
#define CIN0   24
#define CMID   12
#define COUT   24

#define HEAP_SIZE (128 * 1024)


//gemm plano row major en elem_t
static void gemm_plain(const elem_t *A, const elem_t *B, elem_t *C,
                       int I, int J, int K)
{
    for (int i = 0; i < I; ++i) {
        for (int j = 0; j < J; ++j) {
            acc_t acc = 0;
            for (int k = 0; k < K; ++k)
                acc += (acc_t)A[(size_t)i * K + k] * (acc_t)B[(size_t)k * J + j];
            if (acc > elem_t_max) acc = elem_t_max;
            if (acc < elem_t_min) acc = elem_t_min;
            C[(size_t)i * J + j] = (elem_t)acc;
        }
    }
}

int main(void) {

    printf("=== Baseline bare-metal: matvec ChannelMLP (sin acelerar) ===\n");
    printf("SZCH=%d  L=%d  CIN0=%d  CMID=%d  COUT=%d\n", SZCH, L, CIN0, CMID, COUT);
    printf("Formato: int8 (elem_t), acumulador int32 (acc_t)\n");
    printf("Granularidad: GEMM apilado, I=L=%d\n\n", L);
  
    // allocation
    static uint8_t heap[HEAP_SIZE];
    elem_t *A0 = (elem_t *)(&heap[0]);                 
    elem_t *W1 = (elem_t *)(A0 + (size_t)L * CIN0);    
    elem_t *C1 = (elem_t *)(W1 + (size_t)CIN0 * CMID); 
    elem_t *W2 = (elem_t *)(C1 + (size_t)L * CMID);    
    elem_t *C2 = (elem_t *)(W2 + (size_t)CMID * COUT);
    {
        uint8_t *end = (uint8_t *)(C2 + (size_t)L * COUT);
        if (end >= &heap[HEAP_SIZE]) { printf("ERROR: no cabe en el heap\n"); exit(1); }
    }

    // initialization
    for (int i = 0; i < L; ++i) {
        int ones = i % 8;
        for (int k = 0; k < CIN0; ++k)
            A0[(size_t)i * CIN0 + k] = (elem_t)(k < ones ? 1 : 0);
    }
    for (int k = 0; k < CIN0; ++k)
        for (int j = 0; j < CMID; ++j) W1[(size_t)k * CMID + j] = (elem_t)1;
    for (int k = 0; k < CMID; ++k)
        for (int j = 0; j < COUT; ++j) W2[(size_t)k * COUT + j] = (elem_t)1;
  
    // MEDICION INICIA
    uint64_t c_start = read_cycles();
    uint64_t i_start = read_instret();
    
    gemm_plain(A0, W1, C1, L, CMID, CIN0);
    gemm_plain(C1, W2, C2, L, COUT, CMID);


    uint64_t i_end = read_instret();
    uint64_t c_end = read_cycles();
    // MEDICION ACABA

    uint64_t cycles  = c_end - c_start;
    uint64_t instret = i_end - i_start;
    printf("Baseline matvec: %llu cycles, %llu instructions\n",
           (unsigned long long)cycles, (unsigned long long)instret);
    if (cycles > 0)
        printf("IPC = %d.%03d\n",
               (int)(instret / cycles),
               (int)(((instret * 1000) / cycles) % 1000));
    printf("\n");

    int ok = 1;
    for (int i = 0; i < L && ok; ++i) {
        elem_t expected = (elem_t)(CMID * (i % 8));   /* <=84, no satura */
        for (int j = 0; j < COUT; ++j)
            if (C2[(size_t)i * COUT + j] != expected) { ok = 0; break; }
    }
    printf("Verificacion CPU: %s\n", ok ? "PASS" : "FAIL");

    printf("C2[fila 0] col0..3 = %d %d %d %d  (esperado 0)\n",
           (int)C2[0], (int)C2[1], (int)C2[2], (int)C2[3]);
    if (L > 5)
        printf("C2[fila 5] col0..3 = %d %d %d %d  (esperado 60)\n",
               (int)C2[(size_t)5*COUT+0], (int)C2[(size_t)5*COUT+1],
               (int)C2[(size_t)5*COUT+2], (int)C2[(size_t)5*COUT+3]);
    printf("C2[fila %d] col0..3 = %d %d %d %d  (esperado %d)\n", L-1,
           (int)C2[(size_t)(L-1)*COUT+0], (int)C2[(size_t)(L-1)*COUT+1],
           (int)C2[(size_t)(L-1)*COUT+2], (int)C2[(size_t)(L-1)*COUT+3],
           CMID * ((L-1) % 8));

    printf("\n=== Fin baseline ===\n");
    return ok ? 0 : 1;
}
