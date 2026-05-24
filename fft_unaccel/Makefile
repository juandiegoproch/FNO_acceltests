# Toolchain definition
CC      = riscv64-unknown-elf-gcc
AR      = riscv64-unknown-elf-ar
OBJDUMP = riscv64-unknown-elf-objdump

# Project files
SRCS   = main.c
TARGET = program.riscv

# KissFFT Paths
KISS_DIR = ./kissfft
KISS_LIB = $(KISS_DIR)/libkissfft-int32_t.a

# Compiler flags 
# REMOVED -specs from here so it isn't parsed twice
CFLAGS  = -march=rv64imafd -mabi=lp64d -mcmodel=medany \
          -O2 -Wall -g -fno-common \
          -I$(KISS_DIR) \
          -DFIXED_POINT=32 \
          -DKISS_FFT_MALLOC=malloc_is_forbidden \
          -Ukiss_fft_free -Dkiss_fft_free\(ptr\)=

# Linker flags 
# Kept safely here where it belongs for the final linking stage
LDFLAGS = -static -specs=htif_nano.specs -T htif.ld

.PHONY: all clean dump kissfft_build

all: kissfft_build $(TARGET)

# Recursive call for Fixed Point 32
kissfft_build:
	@echo "Building KissFFT (Fixed Point 32)..."
	$(MAKE) -C $(KISS_DIR) \
		CC="$(CC)" \
		AR="$(AR)" \
		CFLAGS="$(CFLAGS)" \
		KISSFFT_STATIC=1 \
		KISSFFT_TOOLS=0 \
		KISSFFT_DATATYPE=int32_t \
		all

# Compiles and links directly into the .riscv ELF file
$(TARGET): $(SRCS) kissfft_build
	@echo "Linking $@"
	$(CC) $(CFLAGS) $(SRCS) $(KISS_LIB) $(LDFLAGS) -o $@ -lm

dump: $(TARGET)
	$(OBJDUMP) -D $< > program.dump

clean:
	@echo "Cleaning up..."
	rm -f $(TARGET) program.dump
	$(MAKE) -C $(KISS_DIR) clean