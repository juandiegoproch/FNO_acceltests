set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv)

# Use the exact paths found in your /opt directory
set(CMAKE_C_COMPILER /opt/riscv/bin/riscv32-unknown-elf-gcc)
set(CMAKE_CXX_COMPILER /opt/riscv/bin/riscv32-unknown-elf-g++)

# This is critical for bare-metal: it stops CMake from trying to 
# build a test "Hello World" app which usually fails due to missing linker scripts
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Optional: help CMake find the standard headers for this toolchain
set(CMAKE_SYSROOT /opt/riscv/riscv32-unknown-elf)
