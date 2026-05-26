/* matvec_gemmini.c — Versión ACELERADA con Gemmini de la MISMA operación que
 * matvec_baseline.c. Mismas dimensiones, mismo formato int8, MISMOS valores,
 * misma instrumentación (cycles + instret) => comparación directa.
 * [VERSIÓN PARAMETRIZABLE + cycles & instret]
 *
 * OPERACIÓN (idéntica al baseline)
 *   Capa 1:  C1[L][CMID] = A0[L][CIN0] * W1[CIN0][CMID]   (24 -> 12)
 *   Capa 2:  C2[L][COUT] = C1[L][CMID] * W2[CMID][COUT]   (12 -> 24)
 *   Apilado en I = L. SIN bias, SIN GELU.
 *
 * MAPEO A tiled_matmul_auto(dim_I, dim_J, dim_K, A, B, D, C,
 *                           strideA, strideB, strideD, strideC, ...)
 *   Capa 1: dim_I=L, dim_J=CMID, dim_K=CIN0; A=A0, B=W1, C=C1
 *   Capa 2: dim_I=L, dim_J=COUT, dim_K=CMID; A=C1, B=W2, C=C2
 *   B en layout [K x J]. Sin bias => D=NULL. NO_ACTIVATION, escalas IDENTITY, WS.
 *
 * PADDING: Gemmini DIM=16; CIN0=24 y CMID=12 no son múltiplos de 16, así que
 * tiled_matmul_auto rellena (24->32, 12->16). Trabajo ocioso esperado del array
 * para matrices pequeñas; se reporta como caracterización.
 *
 * NOTA sobre instret en el lado Gemmini: el core retira las instrucciones RoCC
 * que despachan el trabajo al acelerador, NO las MACs internas del array. Por
 * eso instret será MUCHO menor que en el baseline; eso es justamente lo que
 * evidencia el offload al acelerador.
 *
 * PARAMETRIZACIÓN: igual que el baseline, -DSZCH=16 / -DSZCH=32 (ver build).
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "include/gemmini_testutils.h"

#ifndef SZCH
#define SZCH 32
#endif
#define L      (SZCH * SZCH)
#define CIN0   24
#define CMID   12
#define COUT   24

#define HEAP_SIZE (8 * 1024 * 1024)

/* Lectura de instret de 64 bits, simétrica a read_cycles() (misma que baseline). */
static inline uint64_t read_instret(void) {
    uint64_t x;
    asm volatile ("rdinstret %0" : "=r" (x));
    return x;
}

int main(void) {
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) { perror("mlockall failed"); exit(1); }
#endif

    printf("=== Gemmini: matvec ChannelMLP (acelerado) ===\n");
    printf("SZCH=%d  L=%d  CIN0=%d  CMID=%d  COUT=%d\n", SZCH, L, CIN0, CMID, COUT);
    printf("Formato: int8 (elem_t), acumulador int32 (acc_t)\n");
    printf("Granularidad: GEMM apilado, I=L=%d  (DIM=%d, hay padding)\n\n", L, DIM);

    static uint8_t heap[HEAP_SIZE];
    elem_t *A0 = (elem_t *)(&heap[0]);                 /* [L][CIN0]            */
    elem_t *W1 = (elem_t *)(A0 + (size_t)L * CIN0);    /* [CIN0][CMID] (K x J) */
    elem_t *C1 = (elem_t *)(W1 + (size_t)CIN0 * CMID); /* [L][CMID]            */
    elem_t *W2 = (elem_t *)(C1 + (size_t)L * CMID);    /* [CMID][COUT] (K x J) */
    elem_t *C2 = (elem_t *)(W2 + (size_t)CMID * COUT); /* [L][COUT]            */
    {
        uint8_t *end = (uint8_t *)(C2 + (size_t)L * COUT);
        if (end >= &heap[HEAP_SIZE]) { printf("ERROR: no cabe en el heap\n"); exit(1); }
    }

    /* MISMOS valores que el baseline: fila i con (i%8) unos; pesos todos 1. */
    for (int i = 0; i < L; ++i) {
        int ones = i % 8;
        for (int k = 0; k < CIN0; ++k)
            A0[(size_t)i * CIN0 + k] = (elem_t)(k < ones ? 1 : 0);
    }
    for (int k = 0; k < CIN0; ++k)
        for (int j = 0; j < CMID; ++j) W1[(size_t)k * CMID + j] = (elem_t)1;
    for (int k = 0; k < CMID; ++k)
        for (int j = 0; j < COUT; ++j) W2[(size_t)k * COUT + j] = (elem_t)1;

    gemmini_flush(0);

    /* ===================== VENTANA DE MEDICIÓN ========================= */
    uint64_t c_start = read_cycles();
    uint64_t i_start = read_instret();

    /* Capa 1: [L x CMID] = [L x CIN0] * [CIN0 x CMID] */
    tiled_matmul_auto(
        L, CMID, CIN0,                 /* dim_I, dim_J, dim_K */
        A0, W1, NULL, C1,              /* A, B, D(bias)=NULL, C */
        CIN0, CMID, CMID, CMID,        /* stride_A, stride_B, stride_D, stride_C */
        MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
        NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, true,   /* repeating_bias=true (D=NULL, inocuo) */
        false, false,                  /* transpose_A, transpose_B */
        false, false,                  /* full_C, low_D */
        0,
        WS);

    /* [OMIT-2] GELU(C1) se omite a propósito */

    /* Capa 2: [L x COUT] = [L x CMID] * [CMID x COUT] */
    tiled_matmul_auto(
        L, COUT, CMID,                 /* dim_I, dim_J, dim_K */
        C1, W2, NULL, C2,              /* A, B, D=NULL, C */
        CMID, COUT, COUT, COUT,        /* strides */
        MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
        NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, true,
        false, false,
        false, false,
        0,
        WS);

    gemmini_fence();   /* asegurar que todas las ops RoCC terminaron antes de leer cycle */

    uint64_t i_end = read_instret();
    uint64_t c_end = read_cycles();
    /* =================== FIN VENTANA DE MEDICIÓN ======================= */

    uint64_t cycles  = c_end - c_start;
    uint64_t instret = i_end - i_start;
    printf("Gemmini matvec: %llu cycles, %llu instructions\n",
           (unsigned long long)cycles, (unsigned long long)instret);
    if (cycles > 0)
        printf("IPC = %d.%03d\n",
               (int)(instret / cycles),
               (int)(((instret * 1000) / cycles) % 1000));
    printf("\n");

    /* Verificación: misma salida esperada que el baseline => C2[i][j] = CMID*(i%8). */
    int ok = 1;
    for (int i = 0; i < L && ok; ++i) {
        elem_t expected = (elem_t)(CMID * (i % 8));
        for (int j = 0; j < COUT; ++j)
            if (C2[(size_t)i * COUT + j] != expected) { ok = 0; break; }
    }
    printf("Verificacion (vs salida analitica del baseline): %s\n", ok ? "PASS" : "FAIL");

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

    printf("\n=== Fin Gemmini ===\n");
    return ok ? 0 : 1;
}
