#include <stdio.h>
#include <stdint.h>
#include <stdalign.h>
#include "conv1d.h"

#define CHANNELS    24
#define MLP_MID     12
#define KERNEL_SZ   1       
#define SHIFT_Q1516 16   


#define L_16x16  256
#define L_32x32  1024

#define Q1516(real)   ((int32_t)((double)(real) * 65536.0))

#define BIAS_Q32(real)((int64_t)((double)(real) * 4294967296.0))

alignas(8) int32_t main_buffer      [L_32x32][CHANNELS];
alignas(8) int32_t mlp_hidden_buffer[L_32x32][MLP_MID];
alignas(8) int32_t w0[MLP_MID ][CHANNELS];   
alignas(8) int32_t w1[CHANNELS][MLP_MID ];  
alignas(8) int64_t b0[MLP_MID ];             
alignas(8) int64_t b1[CHANNELS];             

uint64_t clock_cycles_start, clock_cycles_end;
uint64_t instructions_start, instructions_end;

static void init_system(void) {

    for (int co = 0; co < MLP_MID; ++co) {
        b0[co] = BIAS_Q32(0.1);
        for (int ci = 0; ci < CHANNELS; ++ci) w0[co][ci] = Q1516(0.25);
    }
    for (int co = 0; co < CHANNELS; ++co) {
        b1[co] = BIAS_Q32(-0.05);
        for (int ci = 0; ci < MLP_MID; ++ci) w1[co][ci] = Q1516(0.5);
    }
    for (int i = 0; i < L_32x32; ++i)
        for (int c = 0; c < CHANNELS; ++c)
            main_buffer[i][c] = (int32_t)((i + c) << 6);
}

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

static void run_profiled_mlp(int L) {
    asm volatile ("csrw 0x320, 0");           /* mcountinhibit = 0 */
    asm volatile ("fence" ::: "memory");
    asm volatile ("rdcycle   %0" : "=r" (clock_cycles_start));
    asm volatile ("rdinstret %0" : "=r" (instructions_start));
    asm volatile ("fence" ::: "memory");

    conv1d_k1_q1516((const int32_t *)main_buffer, L, CHANNELS,
                    (const int32_t *)w0, MLP_MID,
                    b0, SHIFT_Q1516,
                    (int32_t *)mlp_hidden_buffer);

    conv1d_k1_q1516((const int32_t *)mlp_hidden_buffer, L, MLP_MID,
                    (const int32_t *)w1, CHANNELS,
                    b1, SHIFT_Q1516,
                    (int32_t *)main_buffer);

    asm volatile ("fence" ::: "memory");
    asm volatile ("rdcycle   %0" : "=r" (clock_cycles_end));
    asm volatile ("rdinstret %0" : "=r" (instructions_end));
    asm volatile ("fence" ::: "memory");

    uint64_t total_cycles = clock_cycles_end - clock_cycles_start;
    uint64_t total_instr  = instructions_end - instructions_start;

    if ((total_cycles >> 32) | (total_instr >> 32))
        printf("\n[WARNING] Counters exceeded 32-bit limit!\n");

    dump_results(L, total_cycles, total_instr);
}

int main(void) {
    printf("Starting Conv1D Q15.16 Experiment (RV64/Chipyard)\n");

    init_system();
    run_profiled_mlp(L_16x16);

    init_system();
    run_profiled_mlp(L_32x32);

    printf("Execution finished.\n");
    return 0;
}
