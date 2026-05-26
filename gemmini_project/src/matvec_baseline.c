/* matvec_baseline.c — Baseline BARE-METAL (sin acelerar) de la operación
 * matriz-vector del ChannelMLP de un FNO.   [VERSIÓN AJUSTADA: sin saturación]
 *
 * QUÉ MIDE
 *   La mezcla de canales del ChannelMLP, abstraída y aislada:
 *     - Capa 1:  CIN0 -> CMID   (24 -> 12)
 *     - Capa 2:  CMID -> COUT   (12 -> 24)
 *   Expresada como GEMM apilando las L posiciones espaciales en el eje de filas
 *   (I = L), que es la forma justa de compararla contra Gemmini (mismo trabajo,
 *   misma granularidad).
 *
 * QUÉ OMITE A PROPÓSITO (presente en el FNO real, NO medido aquí)
 *     [OMIT-1] bias de la capa 1
 *     [OMIT-2] activación GELU entre capa 1 y capa 2
 *     [OMIT-3] bias de la capa 2
 *
 * FORMATO NUMÉRICO
 *   int8 entradas/pesos (elem_t), acumulador int32 (acc_t). Datatype estándar
 *   de la instancia Gemmini del proyecto (LeanGemminiConfig). MISMO formato y
 *   MISMOS valores que matvec_gemmini.c, para poder verificar salida idéntica.
 *
 * VALORES (elegidos para NO saturar y dar salidas distinguibles por fila)
 *   W1 = W2 = 1 (todos unos).
 *   Entrada A0: la fila i tiene exactamente (i % 8) unos al principio y 0 el
 *   resto. Entonces:
 *     C1[i][j] = sum_k A0[i][k] = (i % 8)               (igual para todo j)
 *     C2[i][j] = sum_k C1[i][k] = CMID * (i % 8) = 12*(i%8)  ->  {0,12,...,84}
 *   Todos <= 127: NINGUNA saturación. Cada fila da un valor distinto => la
 *   verificación distingue de verdad (no es todo 127 como en la versión previa).
 *
 * BUILD / RUN: idéntico a main.c (dejar en gemmini_project/src/, make, run).
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "include/gemmini_testutils.h"   /* elem_t (int8), acc_t (int32), read_cycles() */

#define SZCH   4                 /* lado de la malla espacial */
#define L      (SZCH * SZCH)     /* posiciones espaciales = 16 */
#define CIN0   24
#define CMID   12
#define COUT   24

#define HEAP_SIZE (4 * 1024 * 1024)

/* GEMM plano: C[I][J] = A[I][K] * B[K][J], int8 con acumulador int32,
 * saturando la salida a int8 (igual que el datapath de Gemmini accType->inputType).
 * Row-major: A[i*K+k], B[k*J+j], C[i*J+j]. */
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
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) { perror("mlockall failed"); exit(1); }
#endif

    printf("=== Baseline bare-metal: matvec ChannelMLP (sin acelerar) ===\n");
    printf("SZCH=%d  L=%d  CIN0=%d  CMID=%d  COUT=%d\n", SZCH, L, CIN0, CMID, COUT);
    printf("Formato: int8 (elem_t), acumulador int32 (acc_t)\n");
    printf("Granularidad: GEMM apilado, I=L=%d\n\n", L);

    static uint8_t heap[HEAP_SIZE];
    elem_t *A0 = (elem_t *)(&heap[0]);                 /* [L][CIN0]              */
    elem_t *W1 = (elem_t *)(A0 + (size_t)L * CIN0);    /* [CIN0][CMID] (K x J)   */
    elem_t *C1 = (elem_t *)(W1 + (size_t)CIN0 * CMID); /* [L][CMID]              */
    elem_t *W2 = (elem_t *)(C1 + (size_t)L * CMID);    /* [CMID][COUT] (K x J)   */
    elem_t *C2 = (elem_t *)(W2 + (size_t)CMID * COUT); /* [L][COUT]              */
    {
        uint8_t *end = (uint8_t *)(C2 + (size_t)L * COUT);
        if (end >= &heap[HEAP_SIZE]) { printf("ERROR: no cabe en el heap\n"); exit(1); }
    }

    /* Inicialización: ver cabecera. Fila i con (i%8) unos; pesos todos 1. */
    for (int i = 0; i < L; ++i) {
        int ones = i % 8;
        for (int k = 0; k < CIN0; ++k)
            A0[(size_t)i * CIN0 + k] = (elem_t)(k < ones ? 1 : 0);
    }
    for (int k = 0; k < CIN0; ++k)
        for (int j = 0; j < CMID; ++j) W1[(size_t)k * CMID + j] = (elem_t)1;
    for (int k = 0; k < CMID; ++k)
        for (int j = 0; j < COUT; ++j) W2[(size_t)k * COUT + j] = (elem_t)1;

    /* ===================== VENTANA DE MEDICIÓN ========================= */
    uint64_t start = read_cycles();
    gemm_plain(A0, W1, C1, L, CMID, CIN0);   /* Capa 1 */
    /* [OMIT-1] bias1 ; [OMIT-2] GELU(C1) */
    gemm_plain(C1, W2, C2, L, COUT, CMID);   /* Capa 2 */
    /* [OMIT-3] bias2 */
    uint64_t end = read_cycles();
    /* =================== FIN VENTANA DE MEDICIÓN ======================= */

    printf("Baseline matvec took %llu cycles\n\n", (unsigned long long)(end - start));

    /* Verificación analítica: C2[i][j] esperado = 12*(i%8). */
    int ok = 1;
    for (int i = 0; i < L && ok; ++i) {
        elem_t expected = (elem_t)(CMID * (i % 8));   /* <=84, no satura */
        for (int j = 0; j < COUT; ++j)
            if (C2[(size_t)i * COUT + j] != expected) { ok = 0; break; }
    }
    printf("Verificacion CPU: %s\n", ok ? "PASS" : "FAIL");

    printf("C2[fila 0]  col0..3 = %d %d %d %d  (esperado 0)\n",
           (int)C2[0], (int)C2[1], (int)C2[2], (int)C2[3]);
    printf("C2[fila 5]  col0..3 = %d %d %d %d  (esperado 60)\n",
           (int)C2[(size_t)5*COUT+0], (int)C2[(size_t)5*COUT+1],
           (int)C2[(size_t)5*COUT+2], (int)C2[(size_t)5*COUT+3]);
    printf("C2[fila 15] col0..3 = %d %d %d %d  (esperado 84)\n",
           (int)C2[(size_t)15*COUT+0], (int)C2[(size_t)15*COUT+1],
           (int)C2[(size_t)15*COUT+2], (int)C2[(size_t)15*COUT+3]);

    printf("\n=== Fin baseline ===\n");
    return ok ? 0 : 1;
}
