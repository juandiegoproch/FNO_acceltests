# Toolchain definition
CC      = riscv32-unknown-elf-gcc
AR      = riscv32-unknown-elf-ar
OBJCOPY = riscv32-unknown-elf-objcopy
OBJDUMP = riscv32-unknown-elf-objdump

# Project files
LINKER_SCRIPT = link.ld
SRCS = main.c crt0.S handlers.S syscalls.c vectors.S
TARGET = program

# KissFFT Paths
KISS_DIR = ./kissfft
# Note the name change to match the int32_t build output
KISS_LIB = $(KISS_DIR)/libkissfft-int32_t.a

# Compiler flags
# Added -DFIXED_POINT=32 so the headers know to use int32_t for kiss_fft_scalar
CFLAGS  = -march=rv32imc_zicsr -mabi=ilp32 -ffreestanding -nostartfiles -O0 -g -Wall \
          -I$(KISS_DIR) \
          -DFIXED_POINT=32 \
          -DKISS_FFT_MALLOC=malloc_is_forbidden -Dkiss_fft_free\(ptr\)=

LDFLAGS = -T $(LINKER_SCRIPT)

.PHONY: all clean dump kissfft_build

all: kissfft_build $(TARGET).hex $(TARGET).elf

# Recursive call for Fixed Point 32
kissfft_build:
	@echo "Building KissFFT (Fixed Point 32)..."
	$(MAKE) -C $(KISS_DIR) \
		CC=$(CC) \
		AR=$(AR) \
		KISSFFT_STATIC=1 \
		KISSFFT_TOOLS=0 \
		KISSFFT_DATATYPE=int32_t \
		all

$(TARGET).elf: $(SRCS) $(LINKER_SCRIPT) kissfft_build
	@echo "Linking $@"
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS) $(KISS_LIB) -o $@ -lm

$(TARGET).hex: $(TARGET).elf
	@echo "Generating $@"
	$(OBJCOPY) -O verilog $< $@

dump: $(TARGET).elf
	$(OBJDUMP) -S -l -d $< > $(TARGET).dis

clean:
	@echo "Cleaning up..."
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).dis
	$(MAKE) -C $(KISS_DIR) clean