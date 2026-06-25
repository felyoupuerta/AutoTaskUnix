CC       := gcc
# El flag -I buscar  headers en esas carpetas automáticamente
CFLAGS   := -Wall -Wextra -Wpedantic -O2 -Isrc/common -Isrc/daemon -Isrc/client
LDFLAGS  := -pthread

#DIRS
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


#  compilar ambos binarios
all: $(DAEMON_BIN) $(CLIENT_BIN)
	@echo "===================================================="
	@echo " Compilación completada con éxito.                  "
	@echo " Ejecutables creados en la carpeta '$(BIN_DIR)':    "
	@echo "  - Demonio: ./$(DAEMON_BIN)                       "
	@echo "  - Cliente: ./$(CLIENT_BIN)                       "
	@echo "===================================================="

# compilar solo demonio
daemon: $(DAEMON_BIN)

# compilar solo cliente
client: $(CLIENT_BIN)

# ==============================================================================
#  Reglas de Enlazado (Linker)
# ==============================================================================

$(DAEMON_BIN): $(DAEMON_OBJS)
	@mkdir -p $(BIN_DIR)
	@echo "==> Enlazando Demonio: $@"
	$(CC) $(DAEMON_OBJS) -o $@ $(LDFLAGS)

$(CLIENT_BIN): $(CLIENT_OBJS)
	@mkdir -p $(BIN_DIR)
	@echo "==> Enlazando Cliente: $@"
	$(CC) $(CLIENT_OBJS) -o $@ $(LDFLAGS)

# ==============================================================================
#  Reglas de Compilación (Compilar objetos .o)
# ==============================================================================

# Compilar objetos del Demonio
$(OBJ_DIR)/daemon/%.o: $(SRC_DIR)/daemon/%.c $(COMMON_HEADERS)
	@mkdir -p $(dir $@)
	@echo "-> Compilando objeto del demonio: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Compilar objetos del Cliente
$(OBJ_DIR)/client/%.o: $(SRC_DIR)/client/%.c $(COMMON_HEADERS)
	@mkdir -p $(dir $@)
	@echo "-> Compilando objeto del cliente: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# ==============================================================================
#  Limpieza y Mantenimiento
# ==============================================================================

clean:
	@echo "Limpiando archivos de compilación..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Limpieza completada."

# Evita conflictos si existen archivos con estos nombres en la raíz
.PHONY: all daemon client clean 
