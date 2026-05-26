/* matvec_baseline.c — Baseline BARE-METAL (sin acelerar) de la operación
 * matriz-vector del ChannelMLP de un FNO.
 *
 * QUÉ MIDE
 *   La operación de mezcla de canales del ChannelMLP, abstraída y aislada:
 *     - Capa 1:  24 -> 12   (para cada una de las L posiciones espaciales)
 *     - Capa 2:  12 -> 24   (para cada una de las L posiciones espaciales)
 *   Expresada como GEMM apilando las L posiciones en el eje de filas (I = L),
 *   que es la forma justa de compararla luego contra Gemmini (mismo trabajo,
 *   misma granularidad).
 *
 * QUÉ OMITE A PROPÓSITO (presente en el FNO real, NO medido aquí)
 *     [OMIT-1] bias de la capa 1
 *     [OMIT-2] activación GELU entre capa 1 y capa 2
 *     [OMIT-3] bias de la capa 2
 *   Solo se mide la multiplicación matriz-vector pura.
 *
 * FORMATO NUMÉRICO
 *   int8 para entradas y pesos (elem_t), acumulador int32 (acc_t). Es el
 *   datatype estándar de la instancia Gemmini del proyecto (LeanGemminiConfig:
 *   inputType=int8, accType=int32). Mismo formato que usará la versión Gemmini.
 *
 * GRANULARIDAD (GEMM, I = L)
 *   Capa 1:  C1[L][12] = A0[L][24] * W1[24][12]
 *   Capa 2:  C2[L][24] = C1[L][12] * W2[12][24]
 *   donde:
 *     A0  = inChannels            (entrada, L posiciones x 24 canales)
 *     W1  = mat1 en layout [K x J] = [24 x 12]   (peso capa 1)
 *     W2  = mat2 en layout [K x J] = [12 x 24]   (peso capa 2)
 *   Nota de layout: el peso se define directamente en [K x J] (lo que Gemmini
 *   espera), NO en la convención PyTorch [out x in]. Como las matrices son
 *   inventadas con valores triviales, esto es libre y se elige por conveniencia.
 *
 * DIMENSIONES (triviales para validar; se escalan después)
 *   szCh = 4   ->   L = szCh*szCh = 16 posiciones espaciales
 *   Cin0 = 24, Cmid = 12, Cout = 24
 *
 * BUILD
 *   Dejar este archivo en  FNO_acceltests/gemmini_project/src/
 *   y compilar con el Makefile existente (mismo flujo que main.c):
 *       cd ~/FNO_acceltests/gemmini_project && make clean && make
 *   Produce:  matvec_baseline-baremetal
 *   Correr (igual que el test que ya funciona):
 *       cd ~/chipyard/sims/verilator
 *       make run-binary CONFIG=fnoaccel.RocketFFTGemminiConfig VERILATOR_THREADS=4 \
 *         BINARY=~/FNO_acceltests/gemmini_project/matvec_baseline-baremetal \
 *         LOADMEM=1 TIMEOUT_CYCLES=100000000000 VERBOSE_FLAGS="" -j8
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "include/gemmini_testutils.h"   /* trae elem_t (int8), acc_t (int32), read_cycles() */

/* ---- Dimensiones del problema (triviales para esta primera validación) ---- */
#define SZCH   4                 /* lado de la malla espacial */
#define L      (SZCH * SZCH)     /* posiciones espaciales = 16 */
#define CIN0   24                /* canales de entrada (capa 1) */
#define CMID   12                /* canales intermedios */
#define COUT   24                /* canales de salida (capa 2) */

#define HEAP_SIZE (4 * 1024 * 1024)

/* ===========================================================================
 *  GEMM plano bare-metal:  C[I][J] = A[I][K] * B[K][J]
 *  - A, B, C en int8 (elem_t), acumulador int32 (acc_t).
 *  - Sin bias, sin activación.
 *  - Saturación del acumulador int32 al rango int8 a la salida (igual que hace
 *    Gemmini al escribir de accType a inputType, para que ambos lados coincidan).
 *  - Layout row-major:  A[i][k] = A[i*K + k],  B[k][j] = B[k*J + j],
 *                       C[i][j] = C[i*J + j].
 * =========================================================================== */
static void gemm_plain(const elem_t *A, const elem_t *B, elem_t *C,
                       int I, int J, int K)
{
    for (int i = 0; i < I; ++i) {
        for (int j = 0; j < J; ++j) {
            acc_t acc = 0;
            for (int k = 0; k < K; ++k) {
                acc += (acc_t)A[(size_t)i * K + k] * (acc_t)B[(size_t)k * J + j];
            }
            /* Saturar a int8 (mismo comportamiento que el datapath de Gemmini). */
            if (acc > elem_t_max) acc = elem_t_max;
            if (acc < elem_t_min) acc = elem_t_min;
            C[(size_t)i * J + j] = (elem_t)acc;
        }
    }
}

int main(void) {
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
        perror("mlockall failed");
        exit(1);
    }
#endif

    printf("=== Baseline bare-metal: matvec ChannelMLP (sin acelerar) ===\n");
    printf("SZCH=%d  L=%d  CIN0=%d  CMID=%d  COUT=%d\n",
           SZCH, L, CIN0, CMID, COUT);
    printf("Formato: int8 (elem_t), acumulador int32 (acc_t)\n");
    printf("Granularidad: GEMM apilado, I=L=%d\n\n", L);

    /* ---- Reserva de buffers sobre un heap estático (estilo main.c) -------- */
    static uint8_t heap[HEAP_SIZE];

    elem_t *A0 = (elem_t *)(&heap[0]);                 /* entrada      [L][CIN0]  */
    elem_t *W1 = (elem_t *)(A0 + (size_t)L * CIN0);    /* peso capa 1  [CIN0][CMID] (layout K x J) */
    elem_t *C1 = (elem_t *)(W1 + (size_t)CIN0 * CMID); /* intermedio   [L][CMID]  */
    elem_t *W2 = (elem_t *)(C1 + (size_t)L * CMID);    /* peso capa 2  [CMID][COUT] (layout K x J) */
    elem_t *C2 = (elem_t *)(W2 + (size_t)CMID * COUT); /* salida       [L][COUT]  */

    {
        uint8_t *end = (uint8_t *)(C2 + (size_t)L * COUT);
        if (end >= &heap[HEAP_SIZE]) {
            printf("ERROR: el problema no cabe en el heap\n");
            exit(1);
        }
    }

    /* ---- Inicialización con valores triviales y deterministas ------------- *
     * Mantengo magnitudes pequeñas para que el acumulador int32 NO sature al
     * llegar a la salida int8 (si saturara, todas las salidas serían 127 y la
     * prueba no distinguiría nada). Con estos valores:
     *   A0[i][k] en {0,1,2}   ,  W1 = 1  ->  C1[i][j] = sum_k A0[i][k] (<= ~48)
     * que excede 127 difícilmente; aun así saturamos por seguridad.            */
    for (int i = 0; i < L; ++i)
        for (int k = 0; k < CIN0; ++k)
            A0[(size_t)i * CIN0 + k] = (elem_t)((i + k) % 3);   /* 0,1,2,0,1,2,... */

    for (int k = 0; k < CIN0; ++k)
        for (int j = 0; j < CMID; ++j)
            W1[(size_t)k * CMID + j] = (elem_t)1;               /* todos 1 */

    for (int k = 0; k < CMID; ++k)
        for (int j = 0; j < COUT; ++j)
            W2[(size_t)k * COUT + j] = (elem_t)1;               /* todos 1 */

    /* ===================== VENTANA DE MEDICIÓN ========================= *
     * Solo el cómputo (las dos capas). Init y verificación quedan fuera.   */
    uint64_t start = read_cycles();

    gemm_plain(A0, W1, C1, L, CMID, CIN0);   /* Capa 1: [L][CMID] = [L][CIN0]*[CIN0][CMID] */
    /* [OMIT-1] bias capa 1 */
    /* [OMIT-2] GELU(C1)    */
    gemm_plain(C1, W2, C2, L, COUT, CMID);   /* Capa 2: [L][COUT] = [L][CMID]*[CMID][COUT] */
    /* [OMIT-3] bias capa 2 */

    uint64_t end = read_cycles();
    /* =================== FIN VENTANA DE MEDICIÓN ======================= */

    uint64_t cycles = end - start;
    printf("Baseline matvec took %llu cycles\n\n", (unsigned long long)cycles);

    /* ---- Verificación de correctitud (FUERA de la ventana) ---------------- *
     * Con W1=W2=1, el resultado es analíticamente predecible:
     *   C1[i][j] = sum_{k=0..CIN0-1} A0[i][k]   (igual para todo j)
     *   C2[i][j] = sum_{k=0..CMID-1} C1[i][k] = CMID * C1[i][0]
     * Pero con saturación int8 los valores se recortan a 127. Recalculamos una
     * referencia con la MISMA saturación para comparar bit a bit.             */
    int ok = 1;
    for (int i = 0; i < L && ok; ++i) {
        /* referencia capa 1 (saturada) para la fila i */
        acc_t c1ref[CMID];
        for (int j = 0; j < CMID; ++j) {
            acc_t a = 0;
            for (int k = 0; k < CIN0; ++k) a += (acc_t)A0[(size_t)i*CIN0+k] * 1;
            if (a > elem_t_max) a = elem_t_max;
            if (a < elem_t_min) a = elem_t_min;
            c1ref[j] = a;
        }
        for (int j = 0; j < COUT; ++j) {
            acc_t a = 0;
            for (int k = 0; k < CMID; ++k) a += c1ref[k] * 1;
            if (a > elem_t_max) a = elem_t_max;
            if (a < elem_t_min) a = elem_t_min;
            if (C2[(size_t)i*COUT + j] != (elem_t)a) { ok = 0; break; }
        }
    }
    printf("Verificacion CPU: %s\n", ok ? "PASS" : "FAIL");

    /* Muestra de salida para inspección a ojo (primera y última fila). */
    printf("C2[0][0..3]   = %d %d %d %d\n",
           (int)C2[0], (int)C2[1], (int)C2[2], (int)C2[3]);
    printf("C2[%d][0..3] = %d %d %d %d\n", L-1,
           (int)C2[(size_t)(L-1)*COUT + 0], (int)C2[(size_t)(L-1)*COUT + 1],
           (int)C2[(size_t)(L-1)*COUT + 2], (int)C2[(size_t)(L-1)*COUT + 3]);

    printf("\n=== Fin baseline ===\n");
    return ok ? 0 : 1;
}
