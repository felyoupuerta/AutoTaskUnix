CC       := gcc
CFLAGS   := -Wall -Wextra -Wpedantic -O2 -Isrc/common -Isrc/daemon -Isrc/client
LDFLAGS  := -pthread

# ==============================================================================
#  Control de Verbose (Modo Detallado)
# ==============================================================================
# Por defecto es silencioso (V=0). Si ejecutas 'make V=1', se activará el modo detallado.
ifeq ($(V),1)
    Q :=
else
    Q := @
endif

# DIRS
SRC_DIR  := src
OBJ_DIR  := obj
BIN_DIR  := bin

# Ejecutables
DAEMON_BIN := $(BIN_DIR)/taskd
CLIENT_BIN := $(BIN_DIR)/taskctl

COMMON_HEADERS := $(SRC_DIR)/common/protocol.h $(SRC_DIR)/common/config.h

DAEMON_SRCS    := $(SRC_DIR)/daemon/main.c \
                  $(SRC_DIR)/daemon/scheduler.c \
                  $(SRC_DIR)/daemon/server.c

CLIENT_SRCS    := $(SRC_DIR)/client/main.c \
                  $(SRC_DIR)/client/client.c

# Objetos
DAEMON_OBJS    := $(DAEMON_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
CLIENT_OBJS    := $(CLIENT_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Compilar ambos binarios
all: $(DAEMON_BIN) $(CLIENT_BIN)
	@echo "===================================================="
	@echo " Compilación completada con éxito.                  "
	@echo " Ejecutables creados en la carpeta '$(BIN_DIR)':    "
	@echo "  - Demonio: ./$(DAEMON_BIN)                       "
	@echo "  - Cliente: ./$(CLIENT_BIN)                       "
	@echo "===================================================="

# Compilar solo demonio
daemon: $(DAEMON_BIN)

# Compilar solo cliente
client: $(CLIENT_BIN)

# ==============================================================================
#  Reglas de Enlazado (Linker)
# ==============================================================================

$(DAEMON_BIN): $(DAEMON_OBJS)
	$(Q)mkdir -p $(BIN_DIR)
	@echo "==> Enlazando Demonio: $@"
	$(Q)$(CC) $(DAEMON_OBJS) -o $@ $(LDFLAGS)

$(CLIENT_BIN): $(CLIENT_OBJS)
	$(Q)mkdir -p $(BIN_DIR)
	@echo "==> Enlazando Cliente: $@"
	$(Q)$(CC) $(CLIENT_OBJS) -o $@ $(LDFLAGS)

# ==============================================================================
#  Reglas de Compilación (Compilar objetos .o)
# ==============================================================================

# Compilar objetos del Demonio
$(OBJ_DIR)/daemon/%.o: $(SRC_DIR)/daemon/%.c $(COMMON_HEADERS)
	$(Q)mkdir -p $(dir $@)
	@echo "-> Compilando objeto del demonio: $<"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

# Compilar objetos del Cliente
$(OBJ_DIR)/client/%.o: $(SRC_DIR)/client/%.c $(COMMON_HEADERS)
	$(Q)mkdir -p $(dir $@)
	@echo "-> Compilando objeto del cliente: $<"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

# ==============================================================================
#  Limpieza y Mantenimiento
# ==============================================================================

clean:
	@echo "Limpiando archivos de compilación..."
	$(Q)rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Limpieza completada."

# Evita conflictos si existen archivos con estos nombres en la raíz
.PHONY: all daemon client clean
