#include <stdio.h>
#include <stdint.h>
#include <stdalign.h>
#include "kiss_fft.h"
#include "kiss_fftndr.h"

#define TRUE 1
#define FALSE 0

#define ONE_DIM_FFT_SIZE 0

#define CHANNEL_SIZE 32
#define CHANNEL_COUNT 24
#define SCRATCHPAD_BUFFER_SIZE 62000 //(CHANNEL_SIZE * CHANNEL_SIZE * sizeof(kiss_fft_cpx) + 284)

#define FIXED_POINT_SHIFT 15

alignas(32) uint8_t fft_scratchpad[SCRATCHPAD_BUFFER_SIZE];
alignas(32) uint8_t ifft_scratchpad[SCRATCHPAD_BUFFER_SIZE];
int32_t channels_buffer[CHANNEL_COUNT][CHANNEL_SIZE][CHANNEL_SIZE];
kiss_fft_cpx channels_buffer_fft[CHANNEL_COUNT][CHANNEL_SIZE][CHANNEL_SIZE];

int32_t oned_buffer[ONE_DIM_FFT_SIZE];
kiss_fft_cpx oned_buffer_fft[ONE_DIM_FFT_SIZE];
alignas(32) uint8_t oned_fft_scratchpad[SCRATCHPAD_BUFFER_SIZE];

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

    uint64_t instructions_start,instructions_end;
    uint64_t clock_cycles_start,clock_cycles_end;
    // write 0 to avoid garbage in most significant word
    instructions_start = instructions_end = 0;
    clock_cycles_start = clock_cycles_end = 0;

    asm volatile ("csrw 0x320, 0");

    asm volatile ("rdcycle %0"    : "=r" (*(uint32_t*)&clock_cycles_start));
    asm volatile ("rdcycleh %0"   : "=r" (*((uint32_t*)&clock_cycles_start + 1)));

    kiss_fftndr_cfg channel_fft_config = kiss_fftndr_alloc(dims,2,FALSE,fft_scratchpad,&scratchpad_size);
    kiss_fftndr_cfg channel_ifft_config = kiss_fftndr_alloc(dims,2,TRUE,ifft_scratchpad,&scratchpad_size);

    asm volatile ("rdcycle %0"    : "=r" (*(uint32_t*)&clock_cycles_end));
    asm volatile ("rdcycleh %0"   : "=r" (*((uint32_t*)&clock_cycles_end + 1)));

    asm volatile ("rdinstret %0"  : "=r" (*(uint32_t*)&instructions_end));
    asm volatile ("rdinstreth %0" : "=r" (*((uint32_t*)&instructions_end + 1)));

    printf("All done with fft setup! \n cycles: %lld, instructions: %lld \n",
            clock_cycles_end-clock_cycles_start,
            instructions_end-instructions_start);

    kiss_fftr_cfg oned_fft = kiss_fftr_alloc(ONE_DIM_FFT_SIZE,FALSE,oned_fft_scratchpad,&oned_scratchpad_size);
    if (oned_fft == NULL){
        printf("Buffer is to small. Current size %d, required size %d \n",SCRATCHPAD_BUFFER_SIZE,scratchpad_size);
        exit(0);
    }
    if (channel_fft_config == NULL){
        printf("Buffer is to small. Current size %d, required size %d \n",SCRATCHPAD_BUFFER_SIZE,scratchpad_size);
        exit(0);
    }
    printf("Finished allocation of configs \n");
    for (int c=0; c<CHANNEL_COUNT; c++)
        populateChannel((int32_t*)&channels_buffer[c]);
    printf("All channels populated. Starting FFT-IFFTs\n");


    asm volatile ("csrw 0x320, 0");

    asm volatile ("rdcycle %0"    : "=r" (*(uint32_t*)&clock_cycles_start));
    asm volatile ("rdcycleh %0"   : "=r" (*((uint32_t*)&clock_cycles_start + 1)));

    asm volatile ("rdinstret %0"  : "=r" (*(uint32_t*)&instructions_start));
    asm volatile ("rdinstreth %0" : "=r" (*((uint32_t*)&instructions_start + 1)));

    for (int c=0; c<CHANNEL_COUNT; c++){
        kiss_fftndr(channel_fft_config,(int32_t*)&channels_buffer[c],(kiss_fft_cpx*)&channels_buffer_fft[c]);
        kiss_fftndri(channel_ifft_config,(kiss_fft_cpx*)&channels_buffer_fft[c],(int32_t*)&channels_buffer[c]);
    }

    asm volatile ("rdcycle %0"    : "=r" (*(uint32_t*)&clock_cycles_end));
    asm volatile ("rdcycleh %0"   : "=r" (*((uint32_t*)&clock_cycles_end + 1)));

    asm volatile ("rdinstret %0"  : "=r" (*(uint32_t*)&instructions_end));
    asm volatile ("rdinstreth %0" : "=r" (*((uint32_t*)&instructions_end + 1)));


    printf("All done with ffts! \n cycles: %lld, instructions: %lld \n",
            clock_cycles_end-clock_cycles_start,
            instructions_end-instructions_start);
    /*
    // We do a 6144 FFT as a sanity check
    printf("Performing large one d fft \n");
    scratchpad_size = SCRATCHPAD_BUFFER_SIZE;
    for (int i=0; i<ONE_DIM_FFT_SIZE; i++) oned_buffer[i] = i;
    printf("Populated... \n");

    asm volatile ("rdcycle %0"    : "=r" (*(uint32_t*)&clock_cycles_start));
    asm volatile ("rdcycleh %0"   : "=r" (*((uint32_t*)&clock_cycles_start + 1)));

    asm volatile ("rdinstret %0"  : "=r" (*(uint32_t*)&instructions_start));
    asm volatile ("rdinstreth %0" : "=r" (*((uint32_t*)&instructions_start + 1)));

    // Perform the Large 1D FFT
    kiss_fftr(oned_fft, (int32_t*)oned_buffer, (kiss_fft_cpx*)oned_buffer_fft);

    // --- END MEASUREMENT ---
    asm volatile ("rdcycle %0"    : "=r" (*(uint32_t*)&clock_cycles_end));
    asm volatile ("rdcycleh %0"   : "=r" (*((uint32_t*)&clock_cycles_end + 1)));

    asm volatile ("rdinstret %0"  : "=r" (*(uint32_t*)&instructions_end));
    asm volatile ("rdinstreth %0" : "=r" (*((uint32_t*)&instructions_end + 1)));


    printf("All done with large 1d fft! \n cycles: %lld, instructions: %lld \n",
            clock_cycles_end-clock_cycles_start,
            instructions_end-instructions_start);
    */
    return 0;
}