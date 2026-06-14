# Toolchain definition
CC      = riscv64-unknown-elf-gcc
OBJDUMP = riscv64-unknown-elf-objdump

# Project files
# Added accelerator_fft.c to the source list
SRCS   = main.c accelerator_fft.c
TARGET = program.riscv

# Compiler flags
# Removed KISSFFT defines and include paths
CFLAGS  = -march=rv64imafd -mabi=lp64d -mcmodel=medany \
          -O2 -Wall -g -fno-common

# Linker flags
LDFLAGS = -static -specs=htif_nano.specs -T ../toolchains/libgloss/util/htif.ld

.PHONY: all clean dump

all: $(TARGET)

# Compiles and links the source files into the .riscv ELF file
$(TARGET): $(SRCS)
	@echo "Building Accelerated FFT Project..."
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $@ -lm

# Generates a disassembly file for debugging hardware instructions
dump: $(TARGET)
	@echo "Generating Dump..."
	$(OBJDUMP) -D $< > program.dump

clean:
	@echo "Cleaning up..."
	rm -f $(TARGET) program.dump
