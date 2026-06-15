# ============================================================
# RV-SCNN baremetal build flow (portatil)
# ============================================================
# Compila el .c de ESTA carpeta a ensamblador, lo deja como <TEST>.S,
# crea el test en core-v-verif (con su test.yaml) y lo despliega.
#
# Uso (tras 'cd ~/RV-SCNN && source env.sh'):
#   make            -> compila y despliega el .S al test de core-v-verif
#   make run        -> ademas lanza 'make veri-test' en sim/core
#   make clean      -> borra .s/.S locales
#
# Variables sobreescribibles (p. ej. para otra maquina/usuario):
#   make RVSCNN_ROOT=/ruta/a/RV-SCNN
#   make TEST_NAME=mi_nombre_de_test
# ============================================================

# Raiz del repo RV-SCNN (por defecto ~/RV-SCNN). Otro layout: pasar RVSCNN_ROOT=...
RVSCNN_ROOT ?= $(HOME)/RV-SCNN

# Nombre del test = nombre de esta carpeta. El folder, el .S y el TEST coinciden.
TEST_NAME   ?= $(notdir $(CURDIR))

# Toolchain: tras 'source env.sh' esta en el PATH (no hardcodear /opt/...).
CC := riscv32-corev-elf-gcc
MARCH       ?= rv32imc_zicsr
MABI        ?= ilp32

# Rutas dentro de core-v-verif
CUSTOM_DIR  := $(RVSCNN_ROOT)/core-v-verif/cv32e40p/tests/programs/custom
DST_DIR     := $(CUSTOM_DIR)/$(TEST_NAME)
SIM_DIR     := $(RVSCNN_ROOT)/core-v-verif/cv32e40p/sim/core

# Fuente C de esta carpeta (se espera exactamente uno)
SRC         := $(firstword $(wildcard *.c))

# ------------------------------------------------------------
.PHONY: all compile deploy run clean
all: deploy

# Compila el .c -> <TEST>.S (capital S, imprescindible para core-v-verif).
# -DBAREMETAL es OBLIGATORIO para el baseline (evita sys/mman + mlockall);
# inofensivo para el accel.
compile: $(SRC)
	@echo "[CC] $(SRC) -> $(TEST_NAME).S"
	$(CC) -S -O3 -DBAREMETAL -march=$(MARCH) -mabi=$(MABI) $(SRC) -o $(TEST_NAME).s
	@mv $(TEST_NAME).s $(TEST_NAME).S

# Crea la carpeta del test, le pone test.yaml (de hello-world) si falta,
# limpia restos y copia el .S.
deploy: compile
	@echo "[DEPLOY] -> $(DST_DIR)"
	@mkdir -p $(DST_DIR)
	@if [ ! -f "$(DST_DIR)/test.yaml" ]; then \
		cp "$(CUSTOM_DIR)/hello-world/test.yaml" "$(DST_DIR)/"; \
		echo "[YAML] copiado test.yaml de hello-world"; \
	fi
	@find "$(DST_DIR)" -type f ! -name '*.S' ! -name 'test.yaml' -delete 2>/dev/null || true
	@cp $(TEST_NAME).S "$(DST_DIR)/"
	@echo "[OK] $(TEST_NAME).S desplegado. Corre: make run  (o cd $(SIM_DIR) && make veri-test TEST=$(TEST_NAME))"

# Lanza la simulacion Verilator del test.
run: deploy
	$(MAKE) -C $(SIM_DIR) veri-test TEST=$(TEST_NAME)

clean:
	@rm -f *.s *.S
