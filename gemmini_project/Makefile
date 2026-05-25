# ==============================================================================
# Relative Paths Configuration (Anchored to the internal copy)
# ==============================================================================
GEMMINI_ROOT    := gemmini-rocc-tests
BENCH_COMMON    := $(GEMMINI_ROOT)/riscv-tests/benchmarks/common

# Toolchain
CROSS_COMPILE   := riscv64-unknown-elf-
CC              := $(CROSS_COMPILE)gcc

# ==============================================================================
# Exact CFLAGS Block from Reference (Mapped to the nested folder layout)
# ==============================================================================
CFLAGS := -DPREALLOCATE=1 \
          -DMULTITHREAD=1 \
          -mcmodel=medany \
          -std=gnu99 \
          -O2 \
          -ffast-math \
          -fno-common \
          -fno-builtin-printf \
          -fno-tree-loop-distribute-patterns \
          -march=rv64gc -Wa,-march=rv64gc \
          -lm \
          -lgcc \
          -I. \
          -I$(GEMMINI_ROOT)/riscv-tests \
          -I$(GEMMINI_ROOT)/riscv-tests/env \
          -I$(GEMMINI_ROOT) \
          -I$(BENCH_COMMON) \
          -DPRINT_TILE=0

# Baremetal specifics mapped from the reference target rule
CFLAGS_BAREMETAL := $(CFLAGS) \
                    -nostdlib \
                    -nostartfiles \
                    -static \
                    -T $(BENCH_COMMON)/test.ld \
                    -DBAREMETAL=1

# ==============================================================================
# Target Sources & Initialization Hardware Files
# ==============================================================================
# Pulls the hardware init files from the safely nested original structure
HW_SRCS       := $(wildcard $(BENCH_COMMON)/*.c) $(wildcard $(BENCH_COMMON)/*.S)

# Target Resolution: Look into the local src/ folder
SRCS          := $(wildcard src/*.c)
TARGETS       := $(patsubst src/%.c, %-baremetal, $(SRCS))

# Force Make to resolve dependencies inside the src folder
vpath %.c src

# ==============================================================================
# Build Rules
# ==============================================================================
.PHONY: all clean

all: $(TARGETS)

%-baremetal: %.c
	@echo "Compiling Target using nested repo dependencies: $< -> $@"
	$(CC) $(CFLAGS_BAREMETAL) $< $(HW_SRCS) -o $@

clean:
	@echo "Cleaning compiled binaries..."
	rm -f *-baremetal