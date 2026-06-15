#include <stdio.h>
#include <stdint.h>
#include <stdalign.h>
#include <stdlib.h>
#include "accelerator_fft.h" // Swapped kissfft headers for your hardware header

#define TRUE 1
#define FALSE 0
#define ONE_DIM_FFT_SIZE 0

// You can now change this to 16, 8, etc. The HW will handle it via zero-padding
#define CHANNEL_SIZE 64
#define CHANNEL_COUNT 24
#define SCRATCHPAD_BUFFER_SIZE 62000

#define HW_DIM 64
#define TOTAL_HW_POINTS (HW_DIM * HW_DIM)

#define FIXED_POINT_SHIFT 16 // Adjusted for Q15.16

alignas(32) uint8_t fft_scratchpad[SCRATCHPAD_BUFFER_SIZE];
alignas(32) uint8_t ifft_scratchpad[SCRATCHPAD_BUFFER_SIZE];

// Swapped to hardware-mapped complex type to keep memory contiguous
// Buffer size is fixed to TOTAL_HW_POINTS (4096) to satisfy hardware requirements
alignas(32) cpx_32bq16_accel channels_buffer[CHANNEL_COUNT][TOTAL_HW_POINTS];

void* malloc_is_forbidden(size_t size) {
    printf("ERROR: KissFFT attempted dynamic allocation of %ld bytes!\n", size);
    exit(1);
    return NULL;
}

// Minimal change: Just cast to the new struct type
void populateChannel(cpx_32bq16_accel* channel){
    for (int r = 0; r < HW_DIM; r++) {
        for (int c = 0; c < HW_DIM; c++) {
            int hw_index = r * HW_DIM + c;

            // Map the smaller CHANNEL_SIZE into the fixed 64x64 hardware window
            if (r < CHANNEL_SIZE && c < CHANNEL_SIZE) {
                // Original logic: (i + j) << FIXED_POINT_SHIFT
                int32_t val = (r + c) << FIXED_POINT_SHIFT;
                channel[hw_index].r = (uint32_t)val;
                channel[hw_index].i = 0;
            } else {
                // Zero-pad the remaining hardware capacity to prevent stalls
                channel[hw_index].r = 0;
                channel[hw_index].i = 0;
            }
        }
    }
}

int main() {
    printf("Running 64x64 FFT instruction");
    uint64_t instructions_start, instructions_end;
    uint64_t clock_cycles_start, clock_cycles_end;

    instructions_start = instructions_end = 0;
    clock_cycles_start = clock_cycles_end = 0;

    asm volatile ("csrw 0x320, 0");
    asm volatile ("fence" ::: "memory");
    asm volatile ("rdcycle %0" : "=r" (clock_cycles_start));
    asm volatile ("rdinstret %0" : "=r" (instructions_start));
    asm volatile ("fence" ::: "memory");

    // kiss_fftndr_alloc calls commented out (Zero changes to the surrounding prints)
    /*
    kiss_fftndr_cfg channel_fft_config = kiss_fftndr_alloc(dims,2,FALSE,fft_scratchpad,&scratchpad_size);
    kiss_fftndr_cfg channel_ifft_config = kiss_fftndr_alloc(dims,2,TRUE,ifft_scratchpad,&scratchpad_size);
    */

    asm volatile ("fence" ::: "memory");
    asm volatile ("rdcycle %0"   : "=r" (clock_cycles_end));
    asm volatile ("rdinstret %0" : "=r" (instructions_end));
    asm volatile ("fence" ::: "memory");

    printf("All done with fft setup! \n cycles: %u, instructions: %u \n",
                (uint32_t)(clock_cycles_end - clock_cycles_start),
                (uint32_t)(instructions_end - instructions_start));

    printf("Finished allocation of configs \n");
    for (int c=0; c<CHANNEL_COUNT; c++)
        populateChannel((cpx_32bq16_accel*)&channels_buffer[c]);
    printf("All channels populated. Starting FFT-IFFTs\n");

    asm volatile ("csrw 0x320, 0");
    asm volatile ("fence" ::: "memory");
    asm volatile ("rdcycle %0"   : "=r" (clock_cycles_start));
    asm volatile ("rdinstret %0" : "=r" (instructions_start));
    asm volatile ("fence" ::: "memory");

    for (int c=0; c<CHANNEL_COUNT; c++) {
        printf("channel %d: FFT start\n", c);

        fft64x64_32bq16_accel(channels_buffer[c]);

        printf("channel %d: FFT done\n", c);

        ifft64x64_32bq16_accel(channels_buffer[c]);

        printf("channel %d: IFFT done\n", c);
    }

    asm volatile ("fence" ::: "memory");
    asm volatile ("rdcycle %0"   : "=r" (clock_cycles_end));
    asm volatile ("rdinstret %0" : "=r" (instructions_end));
    asm volatile ("fence" ::: "memory");

    if (((clock_cycles_end - clock_cycles_start) >> 32) | ((instructions_end - instructions_start) >> 32))
        printf("\n[WARNING] Counters exceeded 32-bit limit!\n");

    printf("All done with ffts! \n cycles: %u, instructions: %u \n",
            (uint32_t)(clock_cycles_end - clock_cycles_start),
            (uint32_t)(instructions_end - instructions_start));

    return 0;
}
