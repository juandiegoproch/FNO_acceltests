# ============================================================
# RVSCNN baremetal build flow
# ============================================================

# Toolchain
export CV_SW_TOOLCHAIN := /opt/riscv
export CV_SW_PREFIX    := riscv32-unknown-elf-
export CV_SW_MARCH     := rv32imc_zicsr

CC := $(CV_SW_TOOLCHAIN)/bin/$(CV_SW_PREFIX)gcc

# Source directory
SRC_DIR := .

# Destination inside core-v-verif
DST_DIR := /home/juan-prochazka/Desktop/RI5CY/RV-SCNN/core-v-verif/cv32e40p/tests/programs/custom/matvec_accel16

# All C files
C_SRCS := $(wildcard $(SRC_DIR)/*.c)

# Generated .s files
ASM_SRCS := $(C_SRCS:.c=.s)

# Generated .S files
CAP_ASM := $(C_SRCS:.c=.S)

# ============================================================
# Default target
# ============================================================

all: compile rename deploy clean_local

# ============================================================
# Compile .c -> .s
# ============================================================

compile:
	@echo "========================================"
	@echo "Compiling C -> assembly"
	@echo "========================================"

	@for file in $(C_SRCS); do \
		echo "[CC] $$file"; \
		$(CC) \
			-S \
			-O3 \
			-DBAREMETAL \
			-march=$(CV_SW_MARCH) \
			-mabi=ilp32 \
			$$file; \
	done

# ============================================================
# Rename .s -> .S
# ============================================================

rename:
	@echo ""
	@echo "========================================"
	@echo "Renaming .s -> .S"
	@echo "========================================"

	@for file in $(ASM_SRCS); do \
		newfile=`echo $$file | sed 's/\.s$$/.S/'`; \
		echo "[MV] $$file -> $$newfile"; \
		mv $$file $$newfile; \
	done

# ============================================================
# Clean destination except .S and test.yaml
# ============================================================

clean_dst:
	@echo ""
	@echo "========================================"
	@echo "Cleaning destination folder"
	@echo "========================================"

	@find $(DST_DIR) \
		-type f \
		! -name "*.S" \
		! -name "test.yaml" \
		-delete

# ============================================================
# Deploy .S files
# ============================================================

deploy: clean_dst
	@echo ""
	@echo "========================================"
	@echo "Copying .S files"
	@echo "========================================"

	@for file in $(CAP_ASM); do \
		echo "[CP] $$file -> $(DST_DIR)"; \
		cp $$file $(DST_DIR)/; \
	done

# ============================================================
# Optional local cleanup
# ============================================================

clean_local:
	@echo ""
	@echo "========================================"
	@echo "Removing local generated .S files"
	@echo "========================================"

	@rm -f *.S

# ============================================================
# Full cleanup
# ============================================================

clean:
	rm -f *.s *.S