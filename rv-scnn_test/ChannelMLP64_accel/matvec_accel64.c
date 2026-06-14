#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "chipyard_compatibility.h"
#include "../include/custom.h"


#ifndef SZCH
#define SZCH 64
#endif
#define L      (SZCH * SZCH)
#define CIN0   24
#define CMID   12
#define COUT   24

/* Heap derivado de las dimensiones: misma formula que el baseline64.
 * Cubre exactamente los cinco buffers del layout (A0,W1,C1,W2,C2) + 4096 B
 * de margen. A L=4096 (SZCH=64) son 246336 B de datos + 4096 B = 250432 B
 * (~245 KiB). */
#define HEAP_SIZE ((L*CIN0 + CIN0*CMID + L*CMID + CMID*COUT + L*COUT) + 4096)

/* K maximo de cualquier capa (CIN0=24, CMID=12). Dimensiona los tiles. */
#define MAX_K 32

/* ============================================================
 * Tiles del acelerador.
 *   a_tile / b_tile: [K][4]  (la dimension K la consume el HW)
 *   res_tile       : 4x4 = 16 resultados int32
 * Alineados a 128 como en GEMM_accel/matvec_accel4.c.
 * ============================================================ */
static int8_t  a_tile[MAX_K * 4] __attribute__((aligned(128)));
static int8_t  b_tile[MAX_K * 4] __attribute__((aligned(128)));
static int32_t res_tile[16]      __attribute__((aligned(128)));

/* ============================================================
 * GEMM acelerado con K resuelto en hardware (patron matvec_accel4).
 *
 * El SAU 4x4 mas su LCU/ACU contraen TODA la dimension K en una sola
 * llamada multicycle de SCNN4x4. El software solo recorre la salida en
 * tiles de 4x4 (M y N). NO hay bucle de K ni acumulacion en software.
 *
 * Convenciones de layout (mismas que matvec_accel4.c salvo A):
 *   A : row-major [M][K]  -> A[(fila)*K + k]   (ChannelMLP)  *transpuesto al empacar*
 *   B : row-major [K][N]  -> B[k*N + col]
 *   C : row-major [M][N]  -> C[(fila)*N + col], saturado a int8 (elem_t)
 *
 * Requiere M y N multiplos de 4 (en el ChannelMLP: L, CMID, COUT lo son).
 * K es arbitrario (<= MAX_K); la maneja el hardware.
 * ============================================================ */
void GEMM_Accelerated(int M, int N, int K, elem_t *A, elem_t *B, elem_t *C) {
    for (int ti = 0; ti < M / 4; ti++) {
        for (int tj = 0; tj < N / 4; tj++) {

            for (int k = 0; k < K; k++)
                for (int r = 0; r < 4; r++)
                    a_tile[k * 4 + r] = A[(ti * 4 + r) * K + k];

            for (int k = 0; k < K; k++)
                for (int c = 0; c < 4; c++)
                    b_tile[k * 4 + c] = B[k * N + tj * 4 + c];

            for (int i = 0; i < 16; i++) res_tile[i] = 0;

            /* Orden y modo del test verificado gemm_cnn.c: L_SCNN, luego L_MODE.
             * L_MODE(0,1,0,0) = CNN entero (sin estado spiking). Por tile, para
             * no asumir que la config persiste entre tiles. */
            L_SCNN(K, 4, 4, 0);
            L_MODE(0, 1, 0, 0);
            SCNN4x4(a_tile, b_tile);
            SCNN_WB_INT((int *)res_tile);

            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {
                    int32_t v = res_tile[r * 4 + c];
                    if (v > elem_t_max) v = elem_t_max;
                    if (v < elem_t_min) v = elem_t_min;
                    C[(ti * 4 + r) * N + (tj * 4 + c)] = (elem_t)v;
                }
            }
        }
    }
}

int main(void) {
    printf("=== Accelerated: matvec ChannelMLP (RV-SCNN, K en HW) ===\n");
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

    /* Capa 1: L x CIN0 * CIN0 x CMID -> L x CMID */
    GEMM_Accelerated(L, CMID, CIN0, A0, W1, C1);

    /* Capa 2: L x CMID * CMID x COUT -> L x COUT */
    GEMM_Accelerated(L, COUT, CMID, C1, W2, C2);

    uint64_t i_end = read_instret();
    uint64_t c_end = read_cycles();

    uint64_t cycles  = c_end - c_start;
    uint64_t instret = i_end - i_start;
    printf("Accelerated matvec: %llu cycles, %llu instructions\n",
           (unsigned long long)cycles, (unsigned long long)instret);

    printf("C1[fila1] col0..3 = %d %d %d %d  (esperado 1)\n",
           (int)C1[1*CMID+0],(int)C1[1*CMID+1],(int)C1[1*CMID+2],(int)C1[1*CMID+3]);
    printf("C1[fila7] col0..3 = %d %d %d %d  (esperado 7)\n",
           (int)C1[7*CMID+0],(int)C1[7*CMID+1],(int)C1[7*CMID+2],(int)C1[7*CMID+3]);

    printf("C2[fila1] j0..7  = %d %d %d %d %d %d %d %d  (esperado todos 12)\n",
           (int)C2[1*COUT+0],(int)C2[1*COUT+1],(int)C2[1*COUT+2],(int)C2[1*COUT+3],
           (int)C2[1*COUT+4],(int)C2[1*COUT+5],(int)C2[1*COUT+6],(int)C2[1*COUT+7]);
    printf("C2[fila2] j0..7  = %d %d %d %d %d %d %d %d  (esperado todos 24)\n",
           (int)C2[2*COUT+0],(int)C2[2*COUT+1],(int)C2[2*COUT+2],(int)C2[2*COUT+3],
           (int)C2[2*COUT+4],(int)C2[2*COUT+5],(int)C2[2*COUT+6],(int)C2[2*COUT+7]);

    int ok = 1;
    for (int i = 0; i < L && ok; ++i) {
        elem_t expected = (elem_t)(CMID * (i % 8));
        for (int j = 0; j < COUT; ++j)
            if (C2[(size_t)i * COUT + j] != expected) {
                printf("Primer fallo: i=%d j=%d esperado=%d obtenido=%d\n",
                       i, j, (int)expected, (int)C2[(size_t)i*COUT+j]);
                ok = 0; break;
            }
    }
    printf("Verificacion HW: %s\n", ok ? "PASS" : "FAIL");

    return ok ? 0 : 1;
}
