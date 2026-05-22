#include <stdint.h>
#include <stdio.h>
#include "conv1d.h"

// --- Architecture Constants ---
#define CHANNELS  24
#define MLP_MID   12
#define KERNEL_SZ 1
#define SHIFT_Q15 15

// Experiments
#define L_16x16   256
#define L_32x32   1024

// --- Memory Buffers ---
int16_t main_buffer[L_32x32][CHANNELS]    __attribute__((aligned(4)));
int16_t mlp_hidden_buffer[L_32x32][MLP_MID] __attribute__((aligned(4)));

// --- Weights and Biases (ModuleList 0) ---
int16_t w0[MLP_MID][CHANNELS][KERNEL_SZ]  __attribute__((aligned(4)));
int32_t b0[MLP_MID]                       __attribute__((aligned(4)));
int16_t w1[CHANNELS][MLP_MID][KERNEL_SZ]  __attribute__((aligned(4)));
int32_t b1[CHANNELS]                       __attribute__((aligned(4)));

// --- Instrumentation Globals ---
uint64_t clock_cycles_start, clock_cycles_end;
uint64_t instructions_start, instructions_end;

/**
 * Basic initialization of weights and input
 */
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

/**
 * Execute ChannelMLP with Instrumentation
 */
void run_profiled_mlp(int L) {
    // 1. Reset and Start Instrumentation
    asm volatile ("csrw 0x320, 0"); 

    asm volatile ("rdcycle %0"    : "=r" (*(uint32_t*)&clock_cycles_start));
    asm volatile ("rdcycleh %0"   : "=r" (*((uint32_t*)&clock_cycles_start + 1)));
    
    asm volatile ("rdinstret %0"  : "=r" (*(uint32_t*)&instructions_start));
    asm volatile ("rdinstreth %0" : "=r" (*((uint32_t*)&instructions_start + 1)));

    // --- WORK: ChannelMLP Block using General Q15 Kernel ---
    
    // Stage 0: (24 -> 12)
    // Parameters: stride=1, pad=0, dilation=1
    conv1d_q15(
        (int16_t *)main_buffer, L, CHANNELS, 
        (int16_t *)w0, MLP_MID, KERNEL_SZ, 
        b0, 1, 0, 1, SHIFT_Q15, 
        (int16_t *)mlp_hidden_buffer
    );

    // ReLU Non-linearity
    for (int i = 0; i < L * MLP_MID; i++) {
        int16_t val = ((int16_t*)mlp_hidden_buffer)[i];
        ((int16_t*)mlp_hidden_buffer)[i] = (val < 0) ? 0 : val;
    }

    // Stage 1: (12 -> 24)
    conv1d_q15(
        (int16_t *)mlp_hidden_buffer, L, MLP_MID, 
        (int16_t *)w1, CHANNELS, KERNEL_SZ, 
        b1, 1, 0, 1, SHIFT_Q15, 
        (int16_t *)main_buffer
    );

    // --- END WORK ---

    // 2. Stop Instrumentation
    asm volatile ("rdcycle %0"    : "=r" (*(uint32_t*)&clock_cycles_end));
    asm volatile ("rdcycleh %0"   : "=r" (*((uint32_t*)&clock_cycles_end + 1)));

    asm volatile ("rdinstret %0"  : "=r" (*(uint32_t*)&instructions_end));
    asm volatile ("rdinstreth %0" : "=r" (*((uint32_t*)&instructions_end + 1)));

    // 3. Reporting
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