export PATH="/opt/riscv/bin:$PATH"

riscv32-unknown-elf-gcc -T link.ld -nostartfiles -ffreestanding main.c crt0.S handlers.S syscalls.c vectors.S -o program.elf
riscv32-unknown-elf-objcopy -O verilog program.elf program.hex
