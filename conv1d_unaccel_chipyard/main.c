/* main.c — Driver e instrumentación del baseline conv1d K=1 en Q15.16.
 *
 * Estructura tomada de:
 *   - ../conv1d_unaccel/main.c  (composición ChannelMLP: 24→12 + ReLU + 12→24)
 *   - ../fft_unaccel/main.c      (patrón de instrumentación RV64 con `fence`)
 *
 * Mide la variante K=1 especializada (`conv1d_k1_q1516`), que es exactamente
 * un producto matriz-vector (GEMV) por cada posición espacial — la estructura
 * que el futuro acelerador de convolución debe explotar.
 *
 * El formato numérico es Q15.16 (int32_t, 16 bits fraccionarios) para
 * coherencia con fft_accel/main.c:13.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdalign.h>
#include "conv1d.h"

/* ---- Constantes de arquitectura (mismas que el baseline CV32) ----------- */
#define CHANNELS    24
#define MLP_MID     12
#define KERNEL_SZ   1       /* informativo: la variante k1 ya asume K=1 */
#define SHIFT_Q1516 16      /* bits fraccionarios → shift de salida */

/* Tamaños espaciales: 16x16=256 y 32x32=1024 posiciones aplanadas. */
#define L_16x16  256
#define L_32x32  1024

/* ---- Conversión real → Q15.16 / Q?.32 (compile-time) -------------------- */
/* Q15.16: valor entero = real * 2^16. Producido como literal entero por GCC
 * a partir de la expresión float (constant folding). */
#define Q1516(real)   ((int32_t)((double)(real) * 65536.0))
/* Bias en la escala del acumulador (Q?.32): real * 2^32. */
#define BIAS_Q32(real)((int64_t)((double)(real) * 4294967296.0))

/* ---- Buffers globales (en .bss, alineados a 8 bytes) -------------------- */
/* Tamaño total estático: ~150 KB, cabe holgado en la DRAM del Rocket. */
alignas(8) int32_t main_buffer      [L_32x32][CHANNELS];
alignas(8) int32_t mlp_hidden_buffer[L_32x32][MLP_MID];
alignas(8) int32_t w0[MLP_MID ][CHANNELS];   /* W stage 0: (Cout=12, Cin=24) */
alignas(8) int32_t w1[CHANNELS][MLP_MID ];   /* W stage 1: (Cout=24, Cin=12) */
alignas(8) int64_t b0[MLP_MID ];             /* bias stage 0 en Q?.32 */
alignas(8) int64_t b1[CHANNELS];             /* bias stage 1 en Q?.32 */

/* ---- Contadores de medición --------------------------------------------- */
uint64_t clock_cycles_start, clock_cycles_end;
uint64_t instructions_start, instructions_end;

/* ---- Inicialización ----------------------------------------------------- */
/* Valores REALES (entre paréntesis), escogidos pequeños para que el
 * acumulador NO sature ni en stage 0 ni en stage 1, y para que el resultado
 * final sea claramente distinto de cero (≈ 18 en valor real).
 *
 * Estimación de cotas (peor caso con todos los pesos al mismo signo):
 *   stage 0: acc ≈ Cin · max|x| · |w0| + |b0|
 *          = 24 · ~1.0 · 0.25 + 0.1 ≈ 6.1  (real)
 *          en Q15.16 ≈ 4.0e5; en acc Q?.32 ≈ 2.6e10  (lejos de INT64_MAX=9.2e18)
 *   stage 1: acc ≈ MLP_MID · max|h| · |w1| + |b1|
 *          = 12 · 6.1 · 0.5 + 0.05 ≈ 36.6 (real)
 *          en Q15.16 ≈ 2.4e6; en acc Q?.32 ≈ 1.6e11  (sin riesgo de overflow)
 *   Ningún valor satura int32 (rango ±2.1e9).
 */
static void init_system(void) {
    /* Stage 0 (24 → 12): w0=0.25, b0=0.1 */
    for (int co = 0; co < MLP_MID; ++co) {
        b0[co] = BIAS_Q32(0.1);
        for (int ci = 0; ci < CHANNELS; ++ci) w0[co][ci] = Q1516(0.25);
    }
    /* Stage 1 (12 → 24): w1=0.5, b1=-0.05 */
    for (int co = 0; co < CHANNELS; ++co) {
        b1[co] = BIAS_Q32(-0.05);
        for (int ci = 0; ci < MLP_MID; ++ci) w1[co][ci] = Q1516(0.5);
    }
    /* Entrada x[i][c] = (i+c)/1024 real, codificada como ((i+c)<<6) en Q15.16
     * porque (i+c) * 2^16 / 1024 = (i+c) * 64 = (i+c) << 6.
     * Rango real: [0, ~1.02], casi [-1, 1) que sería el dominio Q1.15. */
    for (int i = 0; i < L_32x32; ++i)
        for (int c = 0; c < CHANNELS; ++c)
            main_buffer[i][c] = (int32_t)((i + c) << 6);
}

/* ---- Volcado de resultados (FUERA de la ventana de medición) ------------
 * Formato parseable por un script Python:
 *   PARAM CHANNELS=24 MLP_MID=12 L=1024 SHIFT=16 SCALE=65536
 *   CYCLES cycles=<u32> instret=<u32>
 *   OUT idx=<n> val=<int32>           ← primeras 16
 *   OUT idx=<n> val=<int32>           ← últimas 16
 *   CHECKSUM L=1024 sum_int64=<i64>
 * El script Python reconstruye `real = val / 65536.0` y compara contra una
 * referencia float64 de NumPy.
 */
static void dump_results(int L, uint64_t cycles, uint64_t instret) {
    const int32_t *out = (const int32_t *)main_buffer;
    const int total = L * CHANNELS;
    const int n_dump = (total < 16) ? total : 16;

    printf("PARAM CHANNELS=%d MLP_MID=%d L=%d SHIFT=%d SCALE=65536\n",
           CHANNELS, MLP_MID, L, SHIFT_Q1516);
    printf("CYCLES cycles=%u instret=%u\n",
           (uint32_t)cycles, (uint32_t)instret);

    for (int i = 0; i < n_dump; ++i)
        printf("OUT idx=%d val=%d\n", i, out[i]);
    for (int i = total - n_dump; i < total; ++i)
        printf("OUT idx=%d val=%d\n", i, out[i]);

    int64_t checksum = 0;
    for (int i = 0; i < total; ++i) checksum += (int64_t)out[i];
    printf("CHECKSUM L=%d sum_int64=%lld\n", L, (long long)checksum);
}

/* ---- Bloque MLP medido --------------------------------------------------
 * Sigue EXACTAMENTE el patrón de instrumentación de ../fft_unaccel/main.c
 * (commit ff665b5, "Fixed benchmark undefined behaviour on perf regs"):
 *   csrw mcountinhibit, 0
 *   fence
 *   rdcycle / rdinstret de 64 bits
 *   fence
 *   <kernel>
 *   fence
 *   rdcycle / rdinstret de 64 bits
 *   fence
 *
 * El kernel medido es: conv1d K=1 (24→12) + ReLU + conv1d K=1 (12→24).
 * Init y volcado quedan fuera de la ventana.
 */
static void run_profiled_mlp(int L) {
    asm volatile ("csrw 0x320, 0");           /* mcountinhibit = 0 */
    asm volatile ("fence" ::: "memory");
    asm volatile ("rdcycle   %0" : "=r" (clock_cycles_start));
    asm volatile ("rdinstret %0" : "=r" (instructions_start));
    asm volatile ("fence" ::: "memory");

    /* Stage 0: (Cin=24 → Cout=12) */
    conv1d_k1_q1516((const int32_t *)main_buffer, L, CHANNELS,
                    (const int32_t *)w0, MLP_MID,
                    b0, SHIFT_Q1516,
                    (int32_t *)mlp_hidden_buffer);

    /* ReLU sobre L·MLP_MID elementos del buffer oculto */
    {
        int32_t *h = (int32_t *)mlp_hidden_buffer;
        const int N = L * MLP_MID;
        for (int i = 0; i < N; ++i) {
            int32_t v = h[i];
            h[i] = (v < 0) ? 0 : v;
        }
    }

    /* Stage 1: (Cin=12 → Cout=24) — escribe encima de main_buffer */
    conv1d_k1_q1516((const int32_t *)mlp_hidden_buffer, L, MLP_MID,
                    (const int32_t *)w1, CHANNELS,
                    b1, SHIFT_Q1516,
                    (int32_t *)main_buffer);

    asm volatile ("fence" ::: "memory");
    asm volatile ("rdcycle   %0" : "=r" (clock_cycles_end));
    asm volatile ("rdinstret %0" : "=r" (instructions_end));
    asm volatile ("fence" ::: "memory");

    /* --- A partir de aquí, FUERA de la ventana de medición --- */

    uint64_t total_cycles = clock_cycles_end - clock_cycles_start;
    uint64_t total_instr  = instructions_end - instructions_start;

    if ((total_cycles >> 32) | (total_instr >> 32))
        printf("\n[WARNING] Counters exceeded 32-bit limit!\n");

    dump_results(L, total_cycles, total_instr);
}

int main(void) {
    printf("Starting Conv1D Q15.16 Experiment (RV64/Chipyard)\n");

    /* Experimento 1: malla 16x16 */
    init_system();
    run_profiled_mlp(L_16x16);

    /* Experimento 2: malla 32x32 */
    init_system();
    run_profiled_mlp(L_32x32);

    printf("Execution finished.\n");
    return 0;
}
