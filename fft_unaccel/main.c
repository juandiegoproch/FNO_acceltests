#include <stdio.h>
#include <stdint.h>
#include <stdalign.h>
#include <stdlib.h> 
#include "kissfft/kiss_fft.h"
#include "kissfft/kiss_fftndr.h"

#define TRUE 1
#define FALSE 0
#define ONE_DIM_FFT_SIZE 0
#define CHANNEL_SIZE 32
#define CHANNEL_COUNT 24
#define SCRATCHPAD_BUFFER_SIZE 62000 
#define FIXED_POINT_SHIFT 15

alignas(32) uint8_t fft_scratchpad[SCRATCHPAD_BUFFER_SIZE];
alignas(32) uint8_t ifft_scratchpad[SCRATCHPAD_BUFFER_SIZE];
int32_t channels_buffer[CHANNEL_COUNT][CHANNEL_SIZE][CHANNEL_SIZE];
kiss_fft_cpx channels_buffer_fft[CHANNEL_COUNT][CHANNEL_SIZE][CHANNEL_SIZE];

int32_t oned_buffer[ONE_DIM_FFT_SIZE];
kiss_fft_cpx oned_buffer_fft[ONE_DIM_FFT_SIZE];
alignas(32) uint8_t oned_fft_scratchpad[SCRATCHPAD_BUFFER_SIZE];

void* malloc_is_forbidden(size_t size) {
    printf("ERROR: KissFFT attempted dynamic allocation of %ld bytes!\n", size);
    exit(1);
    return NULL;
}

void populateChannel(int32_t* channel){
    for (int i=0; i<CHANNEL_SIZE;i++){
        for (int j=0;j<CHANNEL_SIZE;j++){
            channel[i+j*CHANNEL_SIZE] = (i + j) << FIXED_POINT_SHIFT;
        }
    }
}

int main() {
    int dims[2] = {CHANNEL_SIZE,CHANNEL_SIZE};
    size_t scratchpad_size = SCRATCHPAD_BUFFER_SIZE;
    size_t oned_scratchpad_size = SCRATCHPAD_BUFFER_SIZE;

    uint64_t instructions_start, instructions_end;
    uint64_t clock_cycles_start, clock_cycles_end;
    
    instructions_start = instructions_end = 0;
    clock_cycles_start = clock_cycles_end = 0;

    // --- SETUP BENCHMARK ---
    asm volatile ("csrw 0x320, 0");
    asm volatile ("fence" ::: "memory"); // Barrier
    asm volatile ("rdcycle %0" : "=r" (clock_cycles_start));
    asm volatile ("rdinstret %0" : "=r" (instructions_start));
    asm volatile ("fence" ::: "memory"); // Barrier

    kiss_fftndr_cfg channel_fft_config = kiss_fftndr_alloc(dims,2,FALSE,fft_scratchpad,&scratchpad_size);
    kiss_fftndr_cfg channel_ifft_config = kiss_fftndr_alloc(dims,2,TRUE,ifft_scratchpad,&scratchpad_size);

    asm volatile ("fence" ::: "memory"); 
    asm volatile ("rdcycle %0"   : "=r" (clock_cycles_end));
    asm volatile ("rdinstret %0" : "=r" (instructions_end));
    asm volatile ("fence" ::: "memory");

    printf("All done with fft setup! \n cycles: %u, instructions: %u \n",
                (uint32_t)(clock_cycles_end - clock_cycles_start),
                (uint32_t)(instructions_end - instructions_start));

    // Config checks
    kiss_fftr_cfg oned_fft = kiss_fftr_alloc(ONE_DIM_FFT_SIZE,FALSE,oned_fft_scratchpad,&oned_scratchpad_size);
    if (oned_fft == NULL || channel_fft_config == NULL){
        printf("Buffer is too small.\n");
        exit(0);
    }

    printf("Finished allocation of configs \n");
    for (int c=0; c<CHANNEL_COUNT; c++)
        populateChannel((int32_t*)&channels_buffer[c]);
    printf("All channels populated. Starting FFT-IFFTs\n");

    // --- EXECUTION BENCHMARK ---
    asm volatile ("csrw 0x320, 0");
    asm volatile ("fence" ::: "memory");
    asm volatile ("rdcycle %0"   : "=r" (clock_cycles_start));
    asm volatile ("rdinstret %0" : "=r" (instructions_start));
    asm volatile ("fence" ::: "memory");

    for (int c=0; c<CHANNEL_COUNT; c++){
        kiss_fftndr(channel_fft_config,(int32_t*)&channels_buffer[c],(kiss_fft_cpx*)&channels_buffer_fft[c]);
        kiss_fftndri(channel_ifft_config,(kiss_fft_cpx*)&channels_buffer_fft[c],(int32_t*)&channels_buffer[c]);
    }

    asm volatile ("fence" ::: "memory"); // Force math to finish before reading clock
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