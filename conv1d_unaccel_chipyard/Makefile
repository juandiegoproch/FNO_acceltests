# Makefile para conv1d_unaccel_chipyard (port a RV64/Chipyard del baseline
# conv1d). Copia EXACTAMENTE los flags de ../fft_unaccel/Makefile (toolchain,
# -march, -mabi, -mcmodel, -O, -fno-common, -specs=htif_nano.specs,
# -T htif.ld, -static, -lm). Sin kissfft (no se usa).

# Toolchain (igual que fft_unaccel)
CC      = riscv64-unknown-elf-gcc
OBJDUMP = riscv64-unknown-elf-objdump

# Fuentes del port
SRCS   = main.c conv1d.c
TARGET = program.riscv

# Flags de compilación heredados literal de ../fft_unaccel/Makefile:16-17
#   -march=rv64imafdc  ISA con M, A, F, D y compresión C (igual a fft_unaccel)
#   -mabi=lp64d        ABI hard-float doble (long/ptr de 64 bits)
#   -mcmodel=medany    permite código y datos lejos de 0
#   -O0                idéntico al fft_unaccel (commit ff665b5)
#   -fno-common        sin comunes implícitos en .bss
CFLAGS  = -march=rv64imafdc -mabi=lp64d -mcmodel=medany \
          -O0 -Wall -g -fno-common

# Linker flags idénticos a ../fft_unaccel/Makefile:25
#   -static                 sin enlace dinámico (Chipyard bare-metal)
#   -specs=htif_nano.specs  trae crt0 + syscalls HTIF (printf por consola
#                           del simulador, _exit que escribe tohost=1)
#   -T htif.ld              linker script HTIF (symlink a libgloss)
LDFLAGS = -static -specs=htif_nano.specs -T htif.ld

.PHONY: all clean dump

all: $(TARGET)

# Compila y enlaza directamente al ELF de RV64
$(TARGET): $(SRCS)
	@echo "Linking $@"
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $@ -lm

# Dump de desensamblado para inspección
dump: $(TARGET)
	@echo "Generating disassembly..."
	$(OBJDUMP) -D $< > program.dump

clean:
	@echo "Cleaning up..."
	rm -f $(TARGET) program.dump
