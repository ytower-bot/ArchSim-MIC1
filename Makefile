CC = gcc
CFLAGS = -Wall -Wextra -I./include -g
SRC_DIR = src
UTILS_DIR = src/utils
OBJ_DIR = obj

# Lista de todos os arquivos .c
SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(UTILS_DIR)/*.c)
# Gera lista de arquivos .o
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

# Nome do executável
TARGET = mic1_sim

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Regra genérica para compilar .c em .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(UTILS_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET) data/microcode.txt