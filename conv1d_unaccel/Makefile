# Toolchain definition
CC      = riscv32-unknown-elf-gcc
OBJCOPY = riscv32-unknown-elf-objcopy
OBJDUMP = riscv32-unknown-elf-objdump

# Project files
LINKER_SCRIPT = link.ld
TARGET = program

# Explicitly list all source files found in your directory
SRCS = main.c conv1d.c syscalls.c crt0.S handlers.S vectors.S

# Convert the list of source files into object files (.o)
# This handles both .c and .S files
OBJS = main.o conv1d.o syscalls.o crt0.o handlers.o vectors.o

# Compiler flags
# -I. tells the compiler to look for headers (like conv1d.h) in the current directory
CFLAGS  = -march=rv32imc_zicsr -mabi=ilp32 -ffreestanding -nostartfiles -O0 -g -Wall \
          -I.

LDFLAGS = -T $(LINKER_SCRIPT)

.PHONY: all clean dump

all: $(TARGET).hex $(TARGET).elf

# Linking Stage
$(TARGET).elf: $(OBJS) $(LINKER_SCRIPT)
	@echo "Linking $@"
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@ -lm

# Pattern rule for compiling C files
%.o: %.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Pattern rule for assembling S files
%.o: %.S
	@echo "Assembling $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Conversion to Verilog Hex for simulation/FPGA
$(TARGET).hex: $(TARGET).elf
	@echo "Generating $@"
	$(OBJCOPY) -O verilog $< $@

dump: $(TARGET).elf
	$(OBJDUMP) -S -l -d $< > $(TARGET).dis

clean:
	@echo "Cleaning up..."
	rm -f $(OBJS) $(TARGET).elf $(TARGET).hex $(TARGET).dis