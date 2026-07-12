#include <stdint.h>
#include <stdio.h>
#include "conv1d.h"

#define CHANNELS  24
#define MLP_MID   12
#define KERNEL_SZ 1
#define SHIFT_Q15 15

#define L_16x16   256
#define L_32x32   1024


int16_t main_buffer[L_32x32][CHANNELS]    __attribute__((aligned(4)));
int16_t mlp_hidden_buffer[L_32x32][MLP_MID] __attribute__((aligned(4)));

int16_t w0[MLP_MID][CHANNELS][KERNEL_SZ]  __attribute__((aligned(4)));
int32_t b0[MLP_MID]                       __attribute__((aligned(4)));
int16_t w1[CHANNELS][MLP_MID][KERNEL_SZ]  __attribute__((aligned(4)));
int32_t b1[CHANNELS]                       __attribute__((aligned(4)));

uint64_t clock_cycles_start, clock_cycles_end;
uint64_t instructions_start, instructions_end;

void init_system() {
    for (int i = 0; i < MLP_MID; i++) {
        b0[i] = 10;
        for (int j = 0; j < CHANNELS; j++) w0[i][j][0] = 4000; 
    }
    for (int i = 0; i < CHANNELS; i++) {
        b1[i] = -5;
        for (int j = 0; j < MLP_MID; j++) w1[i][j][0] = 6000;
    }
    for (int i = 0; i < L_32x32; i++) {
        for (int c = 0; c < CHANNELS; c++) main_buffer[i][c] = (int16_t)(i + c);
    }
}

void run_profiled_mlp(int L) {
    // Reset and Start Instrumentation
    asm volatile ("csrw 0x320, 0"); 

    asm volatile ("rdcycle %0"    : "=r" (*(uint32_t*)&clock_cycles_start));
    asm volatile ("rdcycleh %0"   : "=r" (*((uint32_t*)&clock_cycles_start + 1)));
    
    asm volatile ("rdinstret %0"  : "=r" (*(uint32_t*)&instructions_start));
    asm volatile ("rdinstreth %0" : "=r" (*((uint32_t*)&instructions_start + 1)));
    conv1d_q15(
        (int16_t *)main_buffer, L, CHANNELS, 
        (int16_t *)w0, MLP_MID, KERNEL_SZ, 
        b0, 1, 0, 1, SHIFT_Q15, 
        (int16_t *)mlp_hidden_buffer
    );

    conv1d_q15(
        (int16_t *)mlp_hidden_buffer, L, MLP_MID, 
        (int16_t *)w1, CHANNELS, KERNEL_SZ, 
        b1, 1, 0, 1, SHIFT_Q15, 
        (int16_t *)main_buffer
    );
    // Stop Instrumentation
    asm volatile ("rdcycle %0"    : "=r" (*(uint32_t*)&clock_cycles_end));
    asm volatile ("rdcycleh %0"   : "=r" (*((uint32_t*)&clock_cycles_end + 1)));

    asm volatile ("rdinstret %0"  : "=r" (*(uint32_t*)&instructions_end));
    asm volatile ("rdinstreth %0" : "=r" (*((uint32_t*)&instructions_end + 1)));

    // Reporting
    uint64_t total_cycles = clock_cycles_end - clock_cycles_start;
    uint64_t total_instr  = instructions_end - instructions_start;
    
    printf("\n--- Metrics for L=%d (Using General conv1d_q15) ---\n", L);
    printf("Total Cycles: %llu\n", total_cycles);
    printf("Total Instr:  %llu\n", total_instr);
    if (total_cycles > 0) {
        printf("IPC:          %d.%02d\n", (int)(total_instr/total_cycles), (int)((total_instr*100/total_cycles)%100));
    }
}

int main() {
    printf("Starting FNO Experiment Trace...\n");

    init_system();
    run_profiled_mlp(L_16x16); // Experiment 1: 16x16 grid

    init_system();
    run_profiled_mlp(L_32x32); // Experiment 2: 32x32 grid

    printf("\nExecution Finished.\n");
    return 0;
}
